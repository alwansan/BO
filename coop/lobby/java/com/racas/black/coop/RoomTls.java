// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.security.*;
import java.security.cert.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import javax.net.ssl.*;

/** Bounded real TLS lobby. No game objects, engine calls, or simulated players. */
public final class RoomTls {
    private RoomTls() {}
    private static final AtomicInteger THREAD_ID=new AtomicInteger();
    private static final SecureRandom RANDOM=new SecureRandom();
    private static long now(){return System.nanoTime()/1_000_000L;}
    private static byte[] nonce(){byte[] b=new byte[16];RANDOM.nextBytes(b);return b;}
    private static Thread thread(Runnable work){Thread t=new Thread(work,"bo-room-"+THREAD_ID.incrementAndGet());t.setDaemon(true);return t;}
    private static void closeSocket(Socket s){try{s.close();}catch(IOException ignored){}}
    private static String[] protocols(String[] supported)throws IOException{
        List<String> result=new ArrayList<>();
        for(String wanted:new String[]{"TLSv1.3","TLSv1.2"})for(String s:supported)if(s.equals(wanted))result.add(s);
        if(result.isEmpty())throw new IOException("TLS 1.2 or later required");return result.toArray(new String[0]);
    }
    private static void pin(String pin)throws IOException {
        if(pin==null||!pin.matches("[0-9]{8}"))throw new IOException("Eight-digit room PIN required");
    }
    public static SSLContext pinnedContext(byte[] expected)throws GeneralSecurityException{
        if(expected==null||expected.length!=32)throw new IllegalArgumentException("Full SHA-256 certificate pin required");
        final byte[] fingerprint=expected.clone();
        X509TrustManager trust=new X509TrustManager(){
            public X509Certificate[] getAcceptedIssuers(){return new X509Certificate[0];}
            public void checkClientTrusted(X509Certificate[] chain,String auth)throws CertificateException{throw new CertificateException("Not a client CA");}
            public void checkServerTrusted(X509Certificate[] chain,String auth)throws CertificateException{
                if(chain==null||chain.length<1||chain.length>8)throw new CertificateException("Bad certificate chain");
                chain[0].checkValidity();
                try{if(!MessageDigest.isEqual(fingerprint,MessageDigest.getInstance("SHA-256").digest(chain[0].getEncoded())))
                    throw new CertificateException("Host fingerprint mismatch");}
                catch(NoSuchAlgorithmException e){throw new CertificateException(e);}
            }
        };
        SSLContext ctx=SSLContext.getInstance("TLS");ctx.init(null,new TrustManager[]{trust},RANDOM);return ctx;
    }
    public static final class Rejected extends IOException {
        private static final long serialVersionUID=1L;
        public final int reason;
        Rejected(int reason){super("Room rejected request ("+reason+")");this.reason=reason;}
    }
    public static final class Server implements AutoCloseable {
        private final SSLServerSocket listener;
        private final Thread acceptor;
        private final ThreadPoolExecutor workers;
        private final ScheduledExecutorService ticker;
        private final Set<SSLSocket> sockets=ConcurrentHashMap.newKeySet();
        private final ConcurrentMap<SSLSocket,Long> pending=new ConcurrentHashMap<>();
        private final Map<Integer,Peer> peers=new LinkedHashMap<>();
        private final Map<String,Attempt> failures=new HashMap<>();
        private final Object lock=new Object();
        private final byte[] session=nonce(), secret;
        private final String host,room;
        private int nextId=2;
        private long revision=1;
        private volatile boolean closed;
        private static final class Attempt {long start;int count;Attempt(long start){this.start=start;}}
        private final class Peer {
            final int id;final String name;final byte[] echo;final SSLSocket socket;
            final ArrayBlockingQueue<byte[]> outbound=new ArrayBlockingQueue<>(8);
            volatile long lastSeen=now();
            volatile Thread writer;
            final AtomicBoolean dead=new AtomicBoolean();
            Peer(int id,RoomWire.Join join,SSLSocket socket){this.id=id;this.name=join.name;this.echo=join.nonce.clone();this.socket=socket;}
            void send(byte[] data){if(!dead.get()&&!outbound.offer(data))stop();}
            void stop(){if(dead.compareAndSet(false,true)){closeSocket(socket);Thread t=writer;if(t!=null)t.interrupt();}}
        }
        public static Server start(SSLContext ctx,InetAddress bind,int port,String host,String room,String pin)throws IOException{
            return new Server(ctx,bind,port,host,room,pin);
        }
        private Server(SSLContext ctx,InetAddress bind,int port,String host,String room,String pin)throws IOException{
            RoomTls.pin(pin);this.host=RoomWire.name(host);this.room=RoomWire.name(room);
            if(bind==null||port<0||port>65535)throw new IOException("Invalid listener address");
            secret=pin.getBytes(StandardCharsets.US_ASCII);
            listener=(SSLServerSocket)ctx.getServerSocketFactory().createServerSocket();
            try{listener.setReuseAddress(false);listener.setEnabledProtocols(protocols(listener.getSupportedProtocols()));listener.bind(new InetSocketAddress(bind,port),8);}
            catch(IOException|RuntimeException e){try{listener.close();}catch(IOException ignored){}throw e;}
            workers=new ThreadPoolExecutor(6,6,0,TimeUnit.SECONDS,new ArrayBlockingQueue<>(8),RoomTls::thread);
            ticker=Executors.newSingleThreadScheduledExecutor(RoomTls::thread);
            acceptor=thread(this::accept);acceptor.start();ticker.scheduleAtFixedRate(this::tick,500,500,TimeUnit.MILLISECONDS);
        }
        public int port(){return listener.getLocalPort();}
        public boolean isRunning(){return !closed;}
        public RoomWire.State snapshot()throws IOException{synchronized(lock){return RoomWire.parseState(RoomWire.decode(state(RoomWire.ROSTER,1,new byte[16])));}}
        private List<RoomWire.Player> players(){
            List<RoomWire.Player> list=new ArrayList<>();list.add(new RoomWire.Player(1,host));
            for(Peer p:peers.values())list.add(new RoomWire.Player(p.id,p.name));return list;
        }
        // Caller holds lock. Only nonblocking queue offers occur under it.
        private byte[] state(int type,int self,byte[] echo)throws IOException{return RoomWire.state(type,session,echo,self,revision,room,players());}
        private void broadcast(){
            List<Peer> slow=new ArrayList<>();
            synchronized(lock){
                for(Peer p:peers.values())try{
                    // Preserve revision ordering with nonblocking queue offers only.
                    if(!p.dead.get()&&!p.outbound.offer(state(RoomWire.ROSTER,p.id,p.echo)))slow.add(p);
                }catch(IOException e){slow.add(p);}
            }
            for(Peer p:slow)p.stop();
        }
        private boolean limited(String ip){
            long time=now();synchronized(lock){Attempt a=failures.get(ip);return a!=null&&time-a.start<60000&&a.count>=5;}
        }
        private void badPin(String ip){
            long time=now();synchronized(lock){
                failures.entrySet().removeIf(e->time-e.getValue().start>=60000);
                Attempt a=failures.get(ip);
                if(a==null&&failures.size()<16){a=new Attempt(time);failures.put(ip,a);}
                if(a!=null)a.count++;
            }
        }
        private void accept(){
            while(!closed)try{
                SSLSocket s=(SSLSocket)listener.accept();
                if(closed||sockets.size()>=12){closeSocket(s);continue;}
                sockets.add(s);pending.put(s,now());
                try{workers.execute(()->serve(s));}catch(RejectedExecutionException e){pending.remove(s);sockets.remove(s);closeSocket(s);}
            }catch(IOException e){if(!closed)close();}
        }
        private void reject(DataOutputStream out,int reason)throws IOException{RoomWire.write(out,RoomWire.reject(reason));}
        private void serve(SSLSocket socket){
            Peer peer=null;
            try{
                socket.setSoTimeout(3000);socket.setTcpNoDelay(true);socket.setSendBufferSize(8192);
                socket.setEnabledProtocols(protocols(socket.getSupportedProtocols()));socket.startHandshake();
                DataInputStream in=new DataInputStream(socket.getInputStream());DataOutputStream out=new DataOutputStream(socket.getOutputStream());
                String ip=socket.getInetAddress().getHostAddress();
                if(limited(ip)){reject(out,5);return;}
                RoomWire.Join request;
                try{request=RoomWire.parseJoin(RoomWire.decode(RoomWire.read(in)));}
                catch(RoomWire.Incompatible e){reject(out,2);return;}
                if(!MessageDigest.isEqual(secret,request.pin.getBytes(StandardCharsets.US_ASCII))){badPin(ip);reject(out,1);return;}
                synchronized(lock){
                    if(closed)return;
                    if(peers.size()<3&&nextId<Integer.MAX_VALUE&&revision<0xffffffffL){
                        peer=new Peer(nextId++,request,socket);peers.put(peer.id,peer);revision++;
                        // Fresh queue; no network write occurs under the roster lock.
                        if(!peer.outbound.offer(state(RoomWire.ACCEPT,peer.id,peer.echo)))throw new IOException("Fresh queue full");
                    }
                }
                if(peer==null){reject(out,3);return;}
                pending.remove(socket);
                final Peer owned=peer;
                owned.writer=thread(()->{
                    try{while(!owned.dead.get())RoomWire.write(out,owned.outbound.take());}
                    catch(IOException e){owned.stop();}catch(InterruptedException e){Thread.currentThread().interrupt();owned.stop();}
                });owned.writer.start();broadcast();socket.setSoTimeout(5000);
                long lastSequence=0,window=now()/1000;int packets=0;
                while(!closed&&!owned.dead.get()){
                    RoomWire.Packet p=RoomWire.decode(RoomWire.read(in));
                    long current=now()/1000;if(current!=window){window=current;packets=0;}
                    if(++packets>10)throw new IOException("Peer rate exceeded");
                    if(p.type==RoomWire.LEAVE){RoomWire.parseLeave(p);break;}
                    if(p.type!=RoomWire.PING)throw new IOException("Unexpected peer message");
                    long sequence=RoomWire.parsePulse(p);if(sequence<=lastSequence)throw new IOException("Replayed pulse");
                    lastSequence=sequence;owned.lastSeen=now();owned.send(RoomWire.pulse(RoomWire.PONG,sequence));
                }
            }catch(IOException|RuntimeException ignored){
                // A malformed/untrusted/stalled peer is closed, without logging its PIN or private address.
            }finally{
                if(peer!=null){peer.stop();synchronized(lock){if(peers.remove(peer.id)!=null)revision++;}broadcast();}
                pending.remove(socket);sockets.remove(socket);closeSocket(socket);
            }
        }
        private void tick(){
            if(closed)return;
            long time=now();
            for(Map.Entry<SSLSocket,Long> entry:pending.entrySet())
                if(time-entry.getValue()>5000)closeSocket(entry.getKey());
            List<Peer> stale=new ArrayList<>();
            synchronized(lock){for(Peer p:peers.values())if(time-p.lastSeen>5000)stale.add(p);}
            for(Peer p:stale)p.stop();
            broadcast();
        }
        public void close(){
            synchronized(lock){if(closed)return;closed=true;}
            try{listener.close();}catch(IOException ignored){}
            for(SSLSocket s:sockets)closeSocket(s);
            List<Peer> closing;
            synchronized(lock){closing=new ArrayList<>(peers.values());Arrays.fill(secret,(byte)0);}
            for(Peer p:closing)p.stop();
            ticker.shutdownNow();workers.shutdownNow();acceptor.interrupt();
        }
    }
    public static final class Client implements AutoCloseable {
        private final SSLSocket socket;
        private final DataInputStream in;
        private final DataOutputStream out;
        private final Thread reader;
        private final ScheduledExecutorService heartbeat;
        private final AtomicBoolean closed=new AtomicBoolean();
        private final Object lifecycle=new Object();
        private volatile RoomWire.State view;
        private volatile String failure="";
        private volatile long sequence;
        private volatile long lastPong=now();
        public static Client connect(SSLContext context,InetAddress address,int port,String name,String pin)throws IOException{
            return new Client(context,address,port,name,pin);
        }
        private Client(SSLContext context,InetAddress address,int port,String name,String pin)throws IOException{
            RoomTls.pin(pin);RoomWire.name(name);if(address==null||port<1||port>65535)throw new IOException("Invalid peer address");
            byte[] echo=nonce();socket=(SSLSocket)context.getSocketFactory().createSocket();
            ScheduledExecutorService deadline=Executors.newSingleThreadScheduledExecutor(RoomTls::thread);
            deadline.schedule(()->closeSocket(socket),5,TimeUnit.SECONDS);
            try{
                socket.setSoTimeout(3000);socket.setTcpNoDelay(true);socket.setSendBufferSize(8192);
                socket.setEnabledProtocols(protocols(socket.getSupportedProtocols()));socket.connect(new InetSocketAddress(address,port),3000);socket.startHandshake();
                in=new DataInputStream(socket.getInputStream());out=new DataOutputStream(socket.getOutputStream());
                RoomWire.write(out,RoomWire.join(new RoomWire.Join(name,pin,echo)));
                RoomWire.Packet first=RoomWire.decode(RoomWire.read(in));
                if(first.type==RoomWire.REJECT)throw new Rejected(RoomWire.parseReject(first));
                if(first.type!=RoomWire.ACCEPT)throw new IOException("Expected room acceptance");
                view=RoomWire.parseState(first);if(!MessageDigest.isEqual(view.nonce,echo))throw new IOException("Join nonce mismatch");
                socket.setSoTimeout(5000);
            }catch(IOException|RuntimeException e){closeSocket(socket);throw e;}
            finally{deadline.shutdownNow();}
            heartbeat=Executors.newSingleThreadScheduledExecutor(RoomTls::thread);
            reader=thread(this::receive);
            heartbeat.scheduleAtFixedRate(this::ping,0,1000,TimeUnit.MILLISECONDS);reader.start();
        }
        public RoomWire.State snapshot(){RoomWire.State value=view;return value==null?null:RoomWire.copy(value);}
        public String failure(){return failure;}
        public boolean isRunning(){return !closed.get();}
        private void ping(){
            if(closed.get())return;
            try{
                if(now()-lastPong>5000)throw new IOException("Host heartbeat expired");
                if(sequence==Long.MAX_VALUE)throw new IOException("Sequence exhausted");
                RoomWire.write(out,RoomWire.pulse(RoomWire.PING,++sequence));
            }
            catch(IOException e){failure="Connection lost";close();}
        }
        // Package-private seam also exercises a publication delayed until after close.
        void publish(RoomWire.State next){synchronized(lifecycle){if(!closed.get())view=next;}}
        private void receive(){
            try{
                long pong=0;
                while(!closed.get()){
                    RoomWire.Packet packet=RoomWire.decode(RoomWire.read(in));
                    if(packet.type==RoomWire.PONG){long value=RoomWire.parsePulse(packet);if(value<=pong||value>sequence)throw new IOException("Invalid heartbeat");pong=value;lastPong=now();continue;}
                    if(packet.type!=RoomWire.ROSTER)throw new IOException("Unexpected host message");
                    RoomWire.State next=RoomWire.parseState(packet),old=view;
                    if(old==null||next.self!=old.self||!MessageDigest.isEqual(next.session,old.session)||
                       !MessageDigest.isEqual(next.nonce,old.nonce)||next.revision<old.revision||!next.room.equals(old.room))throw new IOException("Room identity changed");
                    publish(next);
                }
            }catch(IOException|RuntimeException e){if(!closed.get())failure="Disconnected";}
            finally{close();}
        }
        public void close(){
            synchronized(lifecycle){if(!closed.compareAndSet(false,true))return;view=null;}
            closeSocket(socket);heartbeat.shutdownNow();reader.interrupt();
        }
    }
}
