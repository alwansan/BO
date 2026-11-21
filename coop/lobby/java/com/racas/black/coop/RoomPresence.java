// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.io.*;import java.net.*;import java.nio.*;import java.nio.charset.*;import java.security.*;import java.util.*;import java.util.concurrent.atomic.*;
/** Bounded presence for running game processes, independent of room membership. */
public final class RoomPresence {
 private RoomPresence(){}
 public static final int PORT=1511,MAX=160;
 private static long now(){return System.nanoTime()/1000000L;}
 public interface Provider{Info get()throws Exception;}
 public static final class Info {
  public final String name,room;public final int role,port,occupied;public final byte[] fingerprint;
  private Info(String name,String room,int role,int port,int occupied,byte[] fp)throws IOException{
   this.name=RoomWire.name(name);this.room=room.isEmpty()?"":RoomWire.name(room);this.role=role;this.port=port;this.occupied=occupied;
   if(role<0||role>2||fp==null||fp.length!=32||(role==1&&(port<1024||port>65535||occupied<1||occupied>4||room.isEmpty()))||(role!=1&&port!=0)||(role==0&&(occupied!=0||!room.isEmpty()))||occupied<0||occupied>4)throw new IOException("Invalid presence");
   fingerprint=fp.clone();
  }
  public static Info idle(String name)throws IOException{return new Info(name,"",0,0,0,new byte[32]);}
  public static Info host(String name,RoomWire.State state,int port,byte[] fp)throws IOException{return new Info(name,state.room,1,port,state.players.size(),fp);}
  public static Info joined(String name,RoomWire.State state)throws IOException{return new Info(name,state.room,2,0,state.players.size(),new byte[32]);}
 }
 public static final class Peer {
  public final InetAddress address;public final Info info;public final boolean compatible;private final byte[] id;
  private Peer(InetAddress address,Info info,boolean compatible,byte[] id){this.address=address;this.info=info;this.compatible=compatible;this.id=id.clone();}
 }
 private static boolean ipv4(InetAddress a){return a instanceof Inet4Address&&!a.isAnyLocalAddress()&&!a.isMulticastAddress();}
 private static void text(DataOutputStream out,String s)throws IOException{byte[] b=s.getBytes(StandardCharsets.UTF_8);if(b.length>32)throw new IOException("Name size");out.writeByte(b.length);out.write(b);}
 private static String text(DataInputStream in,boolean empty)throws IOException{
  int n=in.readUnsignedByte();if(n>32||(!empty&&n==0))throw new IOException("Name size");byte[] b=new byte[n];in.readFully(b);
  String s=StandardCharsets.UTF_8.newDecoder().onMalformedInput(CodingErrorAction.REPORT).onUnmappableCharacter(CodingErrorAction.REPORT).decode(ByteBuffer.wrap(b)).toString();
  if(!s.isEmpty()&&!RoomWire.name(s).equals(s))throw new IOException("Name normalization");return s;
 }
 private static byte[] prefix(int type,int challenge)throws IOException{
  if(challenge==0)throw new IOException("Challenge");ByteArrayOutputStream b=new ByteArrayOutputStream();DataOutputStream out=new DataOutputStream(b);out.writeInt(0x42435052);out.writeByte(1);out.writeByte(type);out.writeInt(challenge);return b.toByteArray();
 }
 private static DataInputStream header(byte[] raw,int type)throws IOException{
  if(raw.length<10||raw.length>MAX)throw new IOException("Presence length");DataInputStream in=new DataInputStream(new ByteArrayInputStream(raw));
  if(in.readInt()!=0x42435052||in.readUnsignedByte()!=1||in.readUnsignedByte()!=type||in.readInt()==0)throw new IOException("Presence header");return in;
 }
 private static byte[] reply(int challenge,byte[] id,Info info)throws IOException{
  ByteArrayOutputStream b=new ByteArrayOutputStream();b.write(prefix(2,challenge));DataOutputStream out=new DataOutputStream(b);
  out.write(id);out.writeByte(info.role);text(out,info.name);text(out,info.room);out.writeShort(info.port);out.writeByte(info.occupied);out.write(info.fingerprint);out.write(RoomWire.image());out.writeShort(RoomWire.PROFILE);return b.toByteArray();
 }
 private static Peer parse(byte[] raw,InetAddress source)throws IOException{
  if(!ipv4(source))throw new IOException("Presence source");DataInputStream in=header(raw,2);byte[] id=new byte[16];in.readFully(id);int role=in.readUnsignedByte();String name=text(in,false),room=text(in,true);int port=in.readUnsignedShort(),count=in.readUnsignedByte();byte[] fp=new byte[32],image=new byte[20];in.readFully(fp);in.readFully(image);int profile=in.readUnsignedShort();if(in.available()!=0)throw new IOException("Trailing presence");
  return new Peer(source,new Info(name,room,role,port,count,fp),MessageDigest.isEqual(image,RoomWire.image())&&profile==RoomWire.PROFILE,id);
 }
 public static final class Service implements AutoCloseable {
  private final DatagramSocket socket;private final byte[] id;private final Provider provider;private final Thread thread;private final AtomicBoolean closed=new AtomicBoolean();
  public static Service start(InetAddress bind,int port,byte[] id,Provider provider)throws IOException{return new Service(bind,port,id,provider);}
  private Service(InetAddress bind,int port,byte[] id,Provider provider)throws IOException{
   if(!(bind instanceof Inet4Address)||port<0||port>65535||(port>0&&port<1024)||id==null||id.length!=16||provider==null)throw new IOException("Presence listener settings");
   this.id=id.clone();this.provider=provider;socket=new DatagramSocket(null);
   try{socket.setReuseAddress(false);socket.bind(new InetSocketAddress(bind,port));socket.setSoTimeout(250);}catch(IOException|RuntimeException e){socket.close();throw e;}
   thread=new Thread(this::run,"bo-presence-listener");thread.setDaemon(true);thread.start();
  }
  public int port(){return socket.getLocalPort();}
  private void run(){
   byte[] buffer=new byte[MAX+1];Map<String,Long> rates=new HashMap<>();long window=0;int sent=0;
   while(!closed.get())try{
    DatagramPacket packet=new DatagramPacket(buffer,buffer.length);socket.receive(packet);if(packet.getLength()!=10||!ipv4(packet.getAddress())||packet.getPort()<1024)continue;
    byte[] raw=Arrays.copyOf(buffer,10);header(raw,1);long time=now();if(time/1000!=window){window=time/1000;sent=0;}if(sent>=16)continue;
    rates.entrySet().removeIf(e->time-e.getValue()>5000);String key=packet.getAddress().getHostAddress();Long previous=rates.get(key);if(previous!=null&&time-previous<300)continue;if(previous==null&&rates.size()>=32)continue;rates.put(key,time);sent++;
    byte[] data=reply(ByteBuffer.wrap(raw,6,4).getInt(),id,provider.get());socket.send(new DatagramPacket(data,data.length,packet.getAddress(),packet.getPort()));
   }catch(SocketTimeoutException expected){}catch(Exception ignored){if(closed.get())break;}
  }
  public void close(){if(closed.compareAndSet(false,true)){socket.close();thread.interrupt();}}
 }
 public static List<Peer> scan(List<InetAddress> targets,int port,int duration,byte[] own,AtomicBoolean cancel)throws IOException{
  if(targets==null||targets.isEmpty()||targets.size()>512||port<1024||port>65535||duration<50||duration>5000||own==null||own.length!=16||cancel==null)throw new IOException("Presence scan settings");
  LinkedHashSet<InetAddress> unique=new LinkedHashSet<>();for(InetAddress a:targets){if(!ipv4(a))throw new IOException("IPv4 target");unique.add(a);}if(cancel.get())return Collections.emptyList();
  int challenge;SecureRandom random=new SecureRandom();do{challenge=random.nextInt();}while(challenge==0);byte[] query=prefix(1,challenge);Map<String,Peer> peers=new LinkedHashMap<>();
  List<InetAddress> destinations=new ArrayList<>(unique);int cursor=0;long end=now()+duration,next=0;
  try(DatagramSocket socket=new DatagramSocket()){
   socket.setBroadcast(true);byte[] buffer=new byte[MAX+1];
   while(!cancel.get()&&!Thread.currentThread().isInterrupted()&&now()<end){
    if(now()>=next){for(int n=0;n<16&&n<destinations.size();n++){
     InetAddress target=destinations.get(cursor++%destinations.size());try{socket.send(new DatagramPacket(query,query.length,target,port));}catch(IOException ignored){}
    }next=now()+125;}
    socket.setSoTimeout((int)Math.max(1,Math.min(125,end-now())));
    try{DatagramPacket packet=new DatagramPacket(buffer,buffer.length);socket.receive(packet);if(packet.getLength()>MAX||packet.getPort()!=port)continue;byte[] raw=Arrays.copyOf(buffer,packet.getLength());if(raw.length<10||ByteBuffer.wrap(raw,6,4).getInt()!=challenge)continue;Peer peer=parse(raw,packet.getAddress());if(MessageDigest.isEqual(peer.id,own))continue;String key=Base64.getEncoder().encodeToString(peer.id);if(peers.size()<32||peers.containsKey(key))peers.put(key,peer);}
    catch(SocketTimeoutException expected){}catch(IOException|RuntimeException ignored){}
   }
  }
  return Collections.unmodifiableList(new ArrayList<>(peers.values()));
 }
 /** Bounded local /24 probes supplement broadcast on VPN interfaces; no unbounded account/subnet enumeration. */
 public static List<InetAddress> targets()throws IOException{
  LinkedHashSet<InetAddress> result=new LinkedHashSet<>(RoomDiscovery.broadcastTargets());
  Enumeration<NetworkInterface> interfaces=NetworkInterface.getNetworkInterfaces();
  while(interfaces!=null&&interfaces.hasMoreElements()&&result.size()<512){NetworkInterface nic=interfaces.nextElement();if(!nic.isUp()||nic.isLoopback())continue;
   for(InterfaceAddress entry:nic.getInterfaceAddresses()){InetAddress address=entry.getAddress();if(!ipv4(address)||address.isLinkLocalAddress())continue;byte[] raw=address.getAddress();for(int host=1;host<255&&result.size()<512;host++){byte[] candidate=raw.clone();candidate[3]=(byte)host;InetAddress target=InetAddress.getByAddress(candidate);if(!target.equals(address))result.add(target);}}
  }
  return Collections.unmodifiableList(new ArrayList<>(result));
 }
}
