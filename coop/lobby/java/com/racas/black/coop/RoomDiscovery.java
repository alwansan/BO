// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.IOException;
import java.net.*;
import java.nio.*;
import java.nio.charset.*;
import java.security.*;
import java.util.*;
import java.util.concurrent.atomic.*;

/** BCLN-v2 discovery. Replies are untrusted hints, never authenticated room membership. */
public final class RoomDiscovery {
    private RoomDiscovery() {}
    public static final int PORT=1510, VERSION=2, MAX_PACKET=69;
    private static final AtomicInteger THREADS=new AtomicInteger();
    private static long now(){return System.nanoTime()/1_000_000L;}
    public static final class Found {
        public final InetAddress address;
        public final int port,occupied,challenge;
        public final String room;
        public final boolean compatible;
        Found(InetAddress address,int port,int occupied,int challenge,String room,boolean compatible){
            this.address=address;this.port=port;this.occupied=occupied;this.challenge=challenge;this.room=room;this.compatible=compatible;
        }
    }
    private static boolean ipv4(InetAddress address){return address instanceof Inet4Address&&!address.isMulticastAddress()&&!address.isAnyLocalAddress();}
    private static ByteBuffer prefix(int type,int challenge,int length)throws IOException{
        if(challenge==0)throw new IOException("Zero discovery challenge");
        ByteBuffer b=ByteBuffer.allocate(length).order(ByteOrder.LITTLE_ENDIAN);
        b.putInt(0x4e4c4342).put((byte)VERSION).put((byte)type).putInt(challenge);return b;
    }
    private static ByteBuffer header(byte[] raw,int type)throws IOException{
        if(raw==null||raw.length<10||raw.length>MAX_PACKET)throw new IOException("Discovery size");
        ByteBuffer b=ByteBuffer.wrap(raw).order(ByteOrder.LITTLE_ENDIAN);
        if(b.getInt()!=0x4e4c4342||b.get()!=VERSION||b.get()!=type||b.getInt()==0)throw new IOException("Discovery header");
        return b;
    }
    static byte[] query(int challenge)throws IOException{return prefix(1,challenge,10).array();}
    private static int parseQuery(byte[] raw)throws IOException{
        if(raw.length!=10)throw new IOException("Query length");ByteBuffer b=header(raw,1);return b.getInt(6);
    }
    static byte[] reply(int challenge,int port,int occupied,String room)throws IOException{
        byte[] name=RoomWire.name(room).getBytes(StandardCharsets.UTF_8);
        if(port<1024||port>65535||occupied<1||occupied>4)throw new IOException("Discovery room fields");
        ByteBuffer b=prefix(2,challenge,37+name.length);
        b.putShort((short)port).put((byte)occupied).put((byte)4).put((byte)name.length).put(name)
            .put(RoomWire.image()).putShort((short)RoomWire.PROFILE);return b.array();
    }
    static Found parseReply(byte[] raw,InetAddress source)throws IOException{
        ByteBuffer b=header(raw,2);if(!ipv4(source)||raw.length<38)throw new IOException("Invalid reply source or size");
        int challenge=b.getInt(6),port=b.getShort()&65535,occupied=b.get()&255,capacity=b.get()&255,n=b.get()&255;
        if(port<1024||capacity!=4||occupied<1||occupied>capacity||n<1||n>32||b.remaining()!=n+22)throw new IOException("Invalid reply fields");
        byte[] bytes=new byte[n];b.get(bytes);
        String name=StandardCharsets.UTF_8.newDecoder().onMalformedInput(CodingErrorAction.REPORT)
            .onUnmappableCharacter(CodingErrorAction.REPORT).decode(ByteBuffer.wrap(bytes)).toString();
        if(!RoomWire.name(name).equals(name))throw new IOException("Noncanonical room name");
        byte[] image=new byte[20];b.get(image);int profile=b.getShort()&65535;
        return new Found(source,port,occupied,challenge,name,MessageDigest.isEqual(image,RoomWire.image())&&profile==RoomWire.PROFILE);
    }
    public static final class Host implements AutoCloseable {
        private final RoomTls.Server room;
        private final DatagramSocket socket;
        private final Thread reader;
        private final AtomicBoolean closed=new AtomicBoolean();
        private final Map<String,Long> seen=new HashMap<>(); // receiver thread only, maximum sixteen IPs
        private long window;
        private int sent;
        public static Host start(RoomTls.Server room,InetAddress bind,int port)throws IOException{return new Host(room,bind,port);}
        private Host(RoomTls.Server room,InetAddress bind,int port)throws IOException{
            if(room==null||!room.isRunning()||!(bind instanceof Inet4Address)||bind.isMulticastAddress()||port<0||port>65535||(port!=0&&port<1024))
                throw new IOException("Invalid discovery listener");
            this.room=room;
            DatagramSocket candidate=null;
            try{
                for(int i=0;i<8;i++){
                    candidate=new DatagramSocket(null);candidate.setReuseAddress(false);candidate.bind(new InetSocketAddress(bind,port));
                    if(candidate.getLocalPort()!=room.port())break;
                    candidate.close();candidate=null;if(port!=0)throw new IOException("Discovery/game port collision");
                }
                if(candidate==null)throw new IOException("No discovery port available");
                candidate.setSoTimeout(250);socket=candidate;
            }catch(IOException|RuntimeException e){if(candidate!=null)candidate.close();throw e;}
            reader=new Thread(this::run,"bo-discovery-"+THREADS.incrementAndGet());reader.setDaemon(true);reader.start();
        }
        public int port(){return socket.getLocalPort();}
        private boolean allow(InetAddress ip){
            long time=now();String key=ip.getHostAddress();
            if(time/1000!=window){window=time/1000;sent=0;}
            if(sent>=8)return false;
            seen.entrySet().removeIf(e->time-e.getValue()>=5000);
            Long previous=seen.get(key);if(previous!=null&&time-previous<900)return false;
            if(previous==null&&seen.size()>=16)return false;
            seen.put(key,time);sent++;return true;
        }
        private void run(){
            byte[] buffer=new byte[MAX_PACKET+1];
            while(!closed.get()&&room.isRunning())try{
                DatagramPacket packet=new DatagramPacket(buffer,buffer.length);socket.receive(packet);
                if(packet.getLength()>MAX_PACKET||packet.getPort()<1024||!ipv4(packet.getAddress()))continue;
                byte[] raw=Arrays.copyOf(packet.getData(),packet.getLength());int challenge=parseQuery(raw);
                if(!allow(packet.getAddress()))continue;
                RoomWire.State current=room.snapshot();byte[] response=reply(challenge,room.port(),current.players.size(),current.room);
                socket.send(new DatagramPacket(response,response.length,packet.getAddress(),packet.getPort()));
            }catch(SocketTimeoutException expected){}
             catch(IOException|RuntimeException ignored){if(closed.get()||!room.isRunning())break;}
            close();
        }
        public void close(){if(closed.compareAndSet(false,true)){socket.close();reader.interrupt();}}
    }
    /** Blocking on a network worker only. Cancellation returns within the receive timeout. */
    public static List<Found> scan(List<InetAddress> targets,int port,int durationMs,AtomicBoolean cancelled)throws IOException{
        if(targets==null||targets.isEmpty()||targets.size()>16||port<1024||port>65535||durationMs<50||durationMs>5000||cancelled==null)
            throw new IOException("Invalid discovery scan");
        Set<InetAddress> unique=new LinkedHashSet<>();for(InetAddress target:targets){if(!ipv4(target))throw new IOException("IPv4 discovery target required");unique.add(target);}
        if(cancelled.get())return Collections.emptyList();
        int challenge;SecureRandom random=new SecureRandom();do{challenge=random.nextInt();}while(challenge==0);
        final byte[] request=query(challenge);Map<String,Found> found=new LinkedHashMap<>();
        long until=now()+durationMs,nextSend=0;
        try(DatagramSocket socket=new DatagramSocket()){
            socket.setBroadcast(true);byte[] buffer=new byte[MAX_PACKET+1];
            while(!cancelled.get()&&!Thread.currentThread().isInterrupted()&&now()<until){
                long time=now();
                if(time>=nextSend){
                    for(InetAddress target:unique)try{socket.send(new DatagramPacket(request,request.length,target,port));}catch(IOException ignored){}
                    nextSend=time+1000;
                }
                socket.setSoTimeout((int)Math.max(1,Math.min(250,until-now())));
                try{
                    DatagramPacket packet=new DatagramPacket(buffer,buffer.length);socket.receive(packet);
                    if(packet.getLength()>MAX_PACKET||packet.getPort()!=port)continue;
                    Found candidate=parseReply(Arrays.copyOf(packet.getData(),packet.getLength()),packet.getAddress());
                    if(candidate.challenge!=challenge)continue;
                    String key=candidate.address.getHostAddress()+":"+candidate.port;
                    if(found.containsKey(key)||found.size()<16)found.put(key,candidate);
                }catch(SocketTimeoutException expected){}
                 catch(IOException|RuntimeException ignored){}
            }
        }
        return Collections.unmodifiableList(new ArrayList<>(found.values()));
    }
    public static List<InetAddress> broadcastTargets()throws SocketException,UnknownHostException{
        Set<InetAddress> result=new LinkedHashSet<>();result.add(InetAddress.getByName("255.255.255.255"));
        Enumeration<NetworkInterface> interfaces=NetworkInterface.getNetworkInterfaces();
        while(interfaces!=null&&interfaces.hasMoreElements()&&result.size()<16){
            NetworkInterface nic=interfaces.nextElement();if(!nic.isUp()||nic.isLoopback())continue;
            for(InterfaceAddress address:nic.getInterfaceAddresses()){
                InetAddress broadcast=address.getBroadcast();if(ipv4(broadcast))result.add(broadcast);if(result.size()>=16)break;
            }
        }
        return Collections.unmodifiableList(new ArrayList<>(result));
    }
}
