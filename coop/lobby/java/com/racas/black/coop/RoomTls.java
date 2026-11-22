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
    // Fixed, non-sensitive event codes ONLY; no peer/user/network/secret data.
    private static void safeEvent(java.util.function.Consumer<String> sink,String code){
        if(sink!=null)try{sink.accept(code);}catch(RuntimeException ignored){}
    }
    private static Thread thread(Runnable work){Thread t=new Thread(work,"bo-room-"+THREAD_ID.incrementAndGet());t.setDaemon(true);return t;}
    private static void closeSocket(Socket s){try{s.close();}catch(IOException ignored){}}
    private static String[] protocols(String[] supported)throws IOException{
        List<String> result=new ArrayList<>();
        for(String wanted:new String[]{"TLSv1.3","TLSv1.2"})for(String s:supported)if(s.equals(wanted))result.add(s);
        if(result.isEmpty())throw new IOException("TLS 1.2 or later required");return result.toArray(new String[0]);
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

        private final Object lock=new Object();
        private volatile java.util.function.Consumer<String> safeEventSink;
        public void setSafeEventSink(java.util.function.Consumer<String> sink){safeEventSink=sink;}
        private void record(String code){safeEvent(safeEventSink,code);}
        private final byte[] session=nonce();
        private final String host,room;
        private int nextId=2;
        private long revision=1;
        private volatile boolean closed;
        // The compact story masks use order within the CURRENT roster rather
        // than RoomWire.Player.id (which never reuses its numeric room ID).
        private RoomStoryWire.Ticket story;
        private final Map<Integer,Integer> storySlots=new HashMap<>();
        private int storyReadyMask;
        private boolean storyCommitted;
        private int lastWorldSnapshotTick,lastNpcSnapshotTick;
        // Pre-JOIN handshake diagnostics only; never store PINs or peer identities.
        private volatile String handshakeFailureKind="none";
        public String handshakeFailureKind(){return handshakeFailureKind;}
        // Every identity comes from the TLS socket, never a network payload.
        public static final class TrustedInput {
            public final int peerId; // authenticated socket's raw wire ID
            public final int playerSlot; // story roster ordinal 2..4
            public final RoomHumanInputWire.Input value;
            private TrustedInput(int wireId,int slot,RoomHumanInputWire.Input value){
                peerId=wireId;playerSlot=slot;this.value=value;
            }
        }
        public static final class TrustedEye {
            public final int peerId;
            public final int playerSlot;
            public final RoomCameraEyeWire.Eye camera;
            private TrustedEye(int wireId,int slot,RoomCameraEyeWire.Eye value){
                peerId=wireId;playerSlot=slot;camera=value;
            }
        }
        /** Non-consuming, fresh source-origin samples from authenticated TLS sockets. */
        public List<TrustedEye> latestVerifiedCameraEyes(){
            synchronized(lock){
                if(closed||story==null||!storyCommitted||
                   revision!=story.rosterRevision)return Collections.emptyList();
                List<TrustedEye> result=new ArrayList<>(3);
                long at=now();
                for(Peer p:peers.values()){
                    RoomCameraEyeWire.Eye eye=p.latestEye;
                    if(eye!=null&&!p.dead.get()&&eye.matches(story)&&
                       storySlots.containsKey(p.id)&&
                       at>=p.latestEyeAt&&at-p.latestEyeAt<=300)
                        result.add(new TrustedEye(p.id,storySlots.get(p.id),eye));
                }
                return result;
            }
        }
        /**
         * Only a VERIFIED host BLACK game-thread callback can supply real
         * original actor positions. Game thread never writes to a socket.
         */
        public boolean offerWorldSnapshot(int tick,
                 List<RoomWorldSnapshotWire.Avatar> avatars)throws IOException{
            synchronized(lock){
                if(closed||story==null||!storyCommitted||
                   revision!=story.rosterRevision||tick<=lastWorldSnapshotTick)
                    return false;
                RoomWorldSnapshotWire.Snapshot snap=
                    RoomWorldSnapshotWire.create(story,tick,avatars);
                // Avatar IDs belong to exact compact story slots, unlike
                // increasing TLS wire IDs after a peer disconnects.
                int seen=0;
                for(RoomWorldSnapshotWire.Avatar a:snap.avatars){
                    if(a.playerId<1||a.playerId>4||
                       !story.includes(a.playerId))
                        throw new IOException("Non-story player in host snapshot");
                    seen|=1<<(a.playerId-1);
                }
                if(seen!=story.participants)
                    throw new IOException("Incomplete compact story snapshot");
                byte[] wire=RoomWorldSnapshotWire.encode(snap);
                lastWorldSnapshotTick=tick;
                for(Peer peer:peers.values())
                    if(!peer.dead.get())peer.latestWorldFrame.set(wire);
                return true;
            }
        }
        // A real ORIGINAL host-source sample: <=32 NPC values, one or
        // two latest-only TLS chunks. Socket writes stay off game thread.
        public boolean offerNpcWorld(int tick,List<RoomNpcWorldWire.Npc> npcs)
                throws IOException {
            synchronized(lock){
                if(closed||story==null||!storyCommitted||
                   revision!=story.rosterRevision||tick<=lastNpcSnapshotTick)
                    return false;
                List<RoomNpcWorldWire.Chunk> chunks=
                    RoomNpcWorldWire.split(story,tick,npcs);
                byte[] first=RoomNpcWorldWire.encode(chunks.get(0));
                byte[] second=chunks.size()==2?
                    RoomNpcWorldWire.encode(chunks.get(1)):null;
                lastNpcSnapshotTick=tick;
                for(Peer p:peers.values())if(!p.dead.get()){
                    p.latestNpcFirst.set(first);
                    p.latestNpcSecond.set(second);
                }
                return true;
            }
        }
        public List<TrustedInput> drainLatestTrustedInputs(){
            synchronized(lock){
                if(closed||story==null||!storyCommitted||revision!=story.rosterRevision)
                    return Collections.emptyList();
                List<TrustedInput> result=new ArrayList<>(3);
                long at=now();
                for(Peer peer:peers.values()){
                    RoomHumanInputWire.Input input=peer.latestInput;
                    peer.latestInput=null;
                    if(input!=null&&!peer.dead.get()&&input.matches(story)&&
                       at>=peer.latestInputAt&&at-peer.latestInputAt<=250&&
                       storySlots.containsKey(peer.id))
                        result.add(new TrustedInput(peer.id,storySlots.get(peer.id),input));
                }
                return result;
            }
        }
        private final class Peer {
            final int id;final String name;final byte[] echo;final SSLSocket socket;
            final ArrayBlockingQueue<byte[]> outbound=new ArrayBlockingQueue<>(8);
            volatile long lastSeen=now();
            volatile Thread writer;
            final AtomicReference<byte[]> latestWorldFrame=new AtomicReference<>();
            final AtomicReference<byte[]> latestNpcFirst=new AtomicReference<>();
            final AtomicReference<byte[]> latestNpcSecond=new AtomicReference<>();
            int lastInputSequence;
            int lastEyeSequence;
            RoomCameraEyeWire.Eye latestEye;
            long latestEyeAt;
            RoomHumanInputWire.Input latestInput;
            long latestInputAt;
            final AtomicBoolean dead=new AtomicBoolean();
            Peer(int id,RoomWire.Join join,SSLSocket socket){this.id=id;this.name=join.name;this.echo=join.nonce.clone();this.socket=socket;}
            void send(byte[] data){if(!dead.get()&&!outbound.offer(data))stop();}
            void stop(){if(dead.compareAndSet(false,true)){closeSocket(socket);Thread t=writer;if(t!=null)t.interrupt();}}
        }
        public static Server start(SSLContext ctx,InetAddress bind,int port,String host,String room)throws IOException{
            return new Server(ctx,bind,port,host,room);
        }
        private Server(SSLContext ctx,InetAddress bind,int port,String host,String room)throws IOException{
            this.host=RoomWire.name(host);this.room=RoomWire.name(room);
            if(bind==null||port<0||port>65535)throw new IOException("Invalid listener address");
            listener=(SSLServerSocket)ctx.getServerSocketFactory().createServerSocket();
            try{// Rebind quickly after owner Host/Leave/Host without waiting for
                // lingering TCP TIME_WAIT endpoints; still reject concurrent LISTEN.
                listener.setReuseAddress(true);
                listener.setEnabledProtocols(protocols(listener.getSupportedProtocols()));
                // On some Android/Linux TCP stacks a just-stopped room can
                // briefly leave its endpoint non-bindable despite reuseaddr.
                // Retry a FIXED private-room port for at most 1.5s on this
                // worker; never silently downgrade to another host/port.
                for(int attempts=0;;attempts++)try{
                    listener.bind(new InetSocketAddress(bind,port),8);break;
                }catch(BindException occupied){
                    if(port==0||attempts>=30)throw occupied;
                    try{Thread.sleep(50);}
                    catch(InterruptedException interrupt){
                        Thread.currentThread().interrupt();
                        throw new IOException("Host listener bind interrupted",interrupt);
                    }
                }}
            catch(IOException|RuntimeException e){try{listener.close();}catch(IOException ignored){}throw e;}
            workers=new ThreadPoolExecutor(6,6,0,TimeUnit.SECONDS,new ArrayBlockingQueue<>(8),RoomTls::thread);
            ticker=Executors.newSingleThreadScheduledExecutor(RoomTls::thread);
            acceptor=thread(this::accept);acceptor.start();ticker.scheduleAtFixedRate(this::tick,500,500,TimeUnit.MILLISECONDS);
        }
        public int port(){return listener.getLocalPort();}
        public boolean isRunning(){return !closed;}
        public RoomStoryWire.Ticket storyTicket(){synchronized(lock){return story;}}
        public boolean storyCommitted(){synchronized(lock){return story!=null&&storyCommitted;}}
        /** A false hostSceneLoaded() may mean WAITING for guests, not
         * rejection. Check the original host READY bit under same lock.
         * Exact ticket identity avoids attributing an old ready after abort.
         */
        public boolean hostLocalStageReady(RoomStoryWire.Ticket exact){
            synchronized(lock){
                return !closed&&story!=null&&exact!=null&&
                    story.same(exact)&&revision==story.rosterRevision&&
                    (storyReadyMask&1)!=0;
            }
        }
        /**
         * Only a future VERIFIED game-thread mission-choice hook may invoke
         * this. The network must never trigger BLACK's scene loader itself.
         */
        public RoomStoryWire.Ticket prepareStory(int mission,int stage,int epoch)throws IOException{
            synchronized(lock){
                if(closed||story!=null||peers.isEmpty()||peers.size()>3)
                    throw new IOException("Story unavailable or already prepared");
                List<RoomWire.Player> roster=players();
                int mask=(1<<roster.size())-1; // host is slot1; 1..3 guests
                long token;
                do{token=RANDOM.nextLong()&Long.MAX_VALUE;}while(token==0);
                RoomStoryWire.Ticket ticket=new RoomStoryWire.Ticket(session,token,revision,
                    mission,stage,epoch,mask);
                byte[] prepare=RoomStoryWire.encode(RoomWire.STORY_PREPARE,ticket);
                // Reserve two frames (fresh roster then prepare) atomically
                // under the same lock as the periodic roster sender.
                for(Peer peer:peers.values())
                    if(peer.dead.get()||peer.outbound.remainingCapacity()<2)
                        throw new IOException("Peer cannot accept story prepare");
                Map<Integer,Integer> slots=new HashMap<>();
                for(int index=1;index<roster.size();index++)
                    slots.put(roster.get(index).id,index+1);
                for(Peer peer:peers.values()){
                    if(!peer.outbound.offer(state(RoomWire.ROSTER,peer.id,peer.echo))||
                       !peer.outbound.offer(prepare))
                        throw new IOException("Peer story prepare queue unexpectedly filled");
                }
                story=ticket;storyReadyMask=0;storyCommitted=false;
                lastWorldSnapshotTick=lastNpcSnapshotTick=0;
                for(Peer p:peers.values()){
                    p.lastInputSequence=0;p.latestInput=null;p.latestWorldFrame.set(null);p.latestNpcFirst.set(null);p.latestNpcSecond.set(null);
                    p.lastEyeSequence=0;p.latestEye=null;p.latestEyeAt=0;
                }
                storySlots.clear();storySlots.putAll(slots);
                record("STORY_HOST_PREPARED");
                return ticket;
            }
        }
        /** Stage unload, peer departure or owner cancellation. NOT a game reset. */
        public boolean cancelStory(){
            synchronized(lock){
                if(closed||story==null)return false;
                abortStoryLocked();return true;
            }
        }
        /** Engine callback only, AFTER local host stage is actually loaded. */
        public boolean hostSceneLoaded(int mission,int stage,int epoch)throws IOException{
            synchronized(lock){
                if(closed||story==null||storyCommitted||
                   mission!=story.mission||stage!=story.stage||epoch!=story.sceneEpoch||
                   (storyReadyMask&1)!=0||revision!=story.rosterRevision)return false;
                storyReadyMask|=1;
                record("STORY_HOST_SCENE_READY");
                return maybeCommitStoryLocked();
            }
        }
        private boolean maybeCommitStoryLocked()throws IOException{
            if(story==null||storyCommitted)return false;
            if(storyReadyMask!=story.participants)return false;
            byte[] commit=RoomStoryWire.encode(RoomWire.STORY_COMMIT,story);
            for(Peer peer:peers.values())
                if(peer.dead.get()||peer.outbound.remainingCapacity()<1){
                    abortStoryLocked();return false;
                }
            for(Peer peer:peers.values())if(!peer.outbound.offer(commit)){
                abortStoryLocked();return false;
            }
            storyCommitted=true;
            record("STORY_HOST_COMMITTED");
            return true;
        }
        private void abortStoryLocked(){
            RoomStoryWire.Ticket old=story;
            story=null;storyReadyMask=0;storyCommitted=false;storySlots.clear();
            lastWorldSnapshotTick=lastNpcSnapshotTick=0;
            for(Peer p:peers.values()){
                p.lastInputSequence=0;p.latestInput=null;p.latestWorldFrame.set(null);p.latestNpcFirst.set(null);p.latestNpcSecond.set(null);
                    p.lastEyeSequence=0;p.latestEye=null;p.latestEyeAt=0;
            }
            if(old!=null)record("STORY_HOST_ABORTED");
            if(old!=null)try{
                byte[] cancel=RoomStoryWire.encode(RoomWire.STORY_ABORT,old);
                for(Peer peer:peers.values())peer.outbound.offer(cancel);
            }catch(IOException ignored){}
        }
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
            boolean tlsEstablished=false;
            try{
                socket.setSoTimeout(3000);socket.setTcpNoDelay(true);socket.setSendBufferSize(8192);
                socket.setEnabledProtocols(protocols(socket.getSupportedProtocols()));socket.startHandshake();
                tlsEstablished=true;
                DataInputStream in=new DataInputStream(socket.getInputStream());DataOutputStream out=new DataOutputStream(socket.getOutputStream());
                RoomWire.Join request;
                try{request=RoomWire.parseJoin(RoomWire.decode(RoomWire.read(in)));}
                catch(RoomWire.Incompatible e){reject(out,2);return;}
                synchronized(lock){
                    if(closed)return;
                    if(peers.size()<3&&nextId<Integer.MAX_VALUE&&revision<0xffffffffL){
                        // Any roster revision change invalidates an uncommitted
                        // barrier; it must not silently start a solo mission.
                        abortStoryLocked();
                        peer=new Peer(nextId++,request,socket);peers.put(peer.id,peer);revision++;
                        // Fresh queue; no network write occurs under the roster lock.
                        if(!peer.outbound.offer(state(RoomWire.ACCEPT,peer.id,peer.echo)))throw new IOException("Fresh queue full");
                    }
                }
                if(peer==null){reject(out,3);return;}
                pending.remove(socket);
                final Peer owned=peer;
                owned.writer=thread(()->{
                    try{
                        while(!owned.dead.get()){
                            byte[] control=owned.outbound.poll(20,TimeUnit.MILLISECONDS);
                            if(control!=null){RoomWire.write(out,control);continue;}
                            byte[] frame=owned.latestWorldFrame.getAndSet(null);
                            if(frame!=null){
                                RoomWorldSnapshotWire.Snapshot sample=
                                    RoomWorldSnapshotWire.parse(RoomWire.decode(frame));
                                synchronized(lock){
                                    if(closed||story==null||!storyCommitted||
                                       revision!=story.rosterRevision||
                                       !sample.matches(story)||owned.dead.get())
                                        continue;
                                }
                                RoomWire.write(out,frame);
                                continue;
                            }
                            // First world/room roster; then original NPC chunks,
                            // all source-pinned to the SAME current story.
                            byte[] a=owned.latestNpcFirst.getAndSet(null);
                            byte[] b=owned.latestNpcSecond.getAndSet(null);
                            for(byte[] chunk:new byte[][]{a,b}){
                                if(chunk==null)continue;
                                RoomNpcWorldWire.Chunk npc=
                                    RoomNpcWorldWire.parse(RoomWire.decode(chunk));
                                synchronized(lock){
                                    if(closed||story==null||!storyCommitted||
                                       revision!=story.rosterRevision||
                                       !npc.matches(story)||owned.dead.get())
                                        continue;
                                }
                                RoomWire.write(out,chunk);
                            }
                        }
                    }catch(IOException e){owned.stop();}
                    catch(InterruptedException e){
                        Thread.currentThread().interrupt();owned.stop();
                    }
                });owned.writer.start();broadcast();socket.setSoTimeout(5000);
                long lastSequence=0,window=now()/1000;int packets=0,inputPackets=0,eyePackets=0;
                while(!closed&&!owned.dead.get()){
                    RoomWire.Packet p=RoomWire.decode(RoomWire.read(in));
                    long current=now()/1000;
                    if(current!=window){window=current;packets=0;inputPackets=0;eyePackets=0;}
                    if(p.type==RoomWire.HUMAN_INPUT){
                        if(++inputPackets>60)throw new IOException("Input rate exceeded");
                        RoomHumanInputWire.Input input=RoomHumanInputWire.parse(p);
                        synchronized(lock){
                            if(closed||story==null||!storyCommitted||
                               revision!=story.rosterRevision||
                               !storySlots.containsKey(owned.id)||!input.matches(story)||
                               input.sequence<=owned.lastInputSequence)
                                throw new IOException("Input before story commit or invalid sequence");
                            owned.lastInputSequence=input.sequence;
                            owned.latestInput=input;owned.latestInputAt=now();
                        }
                        continue;
                    }
                    if(p.type==RoomWire.HUMAN_CAMERA_EYE){
                        if(++eyePackets>15)throw new IOException("Peer camera sample rate exceeded");
                        RoomCameraEyeWire.Eye eye=RoomCameraEyeWire.parse(p);
                        synchronized(lock){
                            if(closed||story==null||!storyCommitted||
                               revision!=story.rosterRevision||
                               !storySlots.containsKey(owned.id)||!eye.matches(story)||
                               eye.sequence<=owned.lastEyeSequence)
                                throw new IOException("Stale or uncommitted original eye");
                            owned.lastEyeSequence=eye.sequence;
                            owned.latestEye=eye;owned.latestEyeAt=now();
                        }
                        continue;
                    }
                    if(++packets>10)throw new IOException("Peer control rate exceeded");
                    if(p.type==RoomWire.LEAVE){RoomWire.parseLeave(p);break;}
                    if(p.type==RoomWire.STORY_READY){
                        RoomStoryWire.Ticket ready=RoomStoryWire.parse(p);
                        synchronized(lock){
                            Integer slot=storySlots.get(owned.id);
                            if(story==null||storyCommitted||revision!=story.rosterRevision||
                               slot==null||!story.same(ready)||
                               (storyReadyMask&(1<<(slot-1)))!=0)
                                throw new IOException("Unexpected, stale or repeated story READY");
                            storyReadyMask|=(1<<(slot-1));
                            record("STORY_HOST_PEER_READY");
                            maybeCommitStoryLocked();
                        }
                        continue;
                    }
                    if(p.type!=RoomWire.PING)throw new IOException("Unexpected peer message");
                    long sequence=RoomWire.parsePulse(p);if(sequence<=lastSequence)throw new IOException("Replayed pulse");
                    lastSequence=sequence;owned.lastSeen=now();owned.send(RoomWire.pulse(RoomWire.PONG,sequence));
                }
            }catch(IOException|RuntimeException ignored){
                if(!tlsEstablished){
                    String message=ignored.getMessage();
                    if(message==null)message="";
                    message=message.replaceAll("[^A-Za-z0-9 ._:/-]","?");
                    if(message.length()>100)message=message.substring(0,100);
                    Throwable cause=ignored.getCause();
                    handshakeFailureKind=ignored.getClass().getSimpleName()+
                        (cause==null?"":("/"+cause.getClass().getSimpleName()))+" "+message;
                }
                // Untrusted peers are closed; no incoming JOIN bytes are logged.
            }finally{
                if(peer!=null){peer.stop();synchronized(lock){if(peers.remove(peer.id)!=null){revision++;abortStoryLocked();}}broadcast();}
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
            synchronized(lock){if(closed)return;closed=true;abortStoryLocked();}
            try{listener.close();}catch(IOException ignored){}
            for(SSLSocket s:sockets)closeSocket(s);
            List<Peer> closing;
            synchronized(lock){closing=new ArrayList<>(peers.values());}
            for(Peer p:closing)p.stop();
            ticker.shutdownNow();workers.shutdownNow();acceptor.interrupt();
        }
    }
    public static final class Client implements AutoCloseable {
        private final SSLSocket socket;
        private final DataInputStream in;
        private final DataOutputStream out;
        private final Thread reader;
        // TLS writes must NEVER run on the verified BLACK game thread.
        // READY is a single, bounded, fail-closed task: no per-frame queueing.
        // Heartbeat uses its existing dedicated background scheduler.
        // Conflated newest-input queue, one dedicated TLS output worker.
        private final ArrayBlockingQueue<byte[]> inputFrames=new ArrayBlockingQueue<>(1);
        private final ArrayBlockingQueue<byte[]> cameraFrames=new ArrayBlockingQueue<>(1);
        private final Thread inputWriter;
        private int lastQueuedInputSequence;
        private int lastQueuedEyeSequence;
        private final AtomicBoolean firstCameraEyeSent=new AtomicBoolean();
        private final AtomicBoolean firstHumanInputSent=new AtomicBoolean();
        private final ThreadPoolExecutor readyWriter=new ThreadPoolExecutor(
            1,1,0L,TimeUnit.MILLISECONDS,new ArrayBlockingQueue<>(1),
            RoomTls::thread,new ThreadPoolExecutor.AbortPolicy());
        private final ScheduledExecutorService heartbeat;
        private final AtomicBoolean closed=new AtomicBoolean();
        private final Object lifecycle=new Object();
        private volatile RoomWire.State view;
        private volatile RoomStoryWire.Ticket story;
        private volatile RoomWorldSnapshotWire.Snapshot worldSnapshot;
        private volatile RoomNpcWorldWire.World npcWorld;
        private RoomNpcWorldWire.Chunk pendingNpc;
        private int lastNpcTick;
        private volatile long lastNpcAt;
        private volatile long lastWorldSnapshotAt;
        private int lastWorldSnapshotTick;
        private volatile boolean storyReadySent,storyCommitted;
        private volatile String failure="";
        private volatile java.util.function.Consumer<String> safeEventSink;
        public void setSafeEventSink(java.util.function.Consumer<String> sink){safeEventSink=sink;}
        private void record(String code){safeEvent(safeEventSink,code);}
        private volatile long sequence;
        private volatile long lastPong=now();
        public static Client connect(SSLContext context,InetAddress address,int port,String name)throws IOException{
            return connect(context,address,port,name,()->false);
        }
        static Client connect(SSLContext context,InetAddress address,int port,String name,java.util.function.BooleanSupplier cancelled)throws IOException{
            return new Client(context,address,port,name,cancelled);
        }
        private Client(SSLContext context,InetAddress address,int port,String name,java.util.function.BooleanSupplier cancelled)throws IOException{
            RoomWire.name(name);if(address==null||port<1||port>65535)throw new IOException("Invalid peer address");
            if(cancelled.getAsBoolean())throw new IOException("Join cancelled");
            byte[] echo=nonce();socket=(SSLSocket)context.getSocketFactory().createSocket();
            ScheduledExecutorService deadline=Executors.newSingleThreadScheduledExecutor(RoomTls::thread);
            deadline.schedule(()->closeSocket(socket),5,TimeUnit.SECONDS);
            deadline.scheduleAtFixedRate(()->{if(cancelled.getAsBoolean())closeSocket(socket);},0,50,TimeUnit.MILLISECONDS);
            try{
                socket.setSoTimeout(3000);socket.setTcpNoDelay(true);socket.setSendBufferSize(8192);
                socket.setEnabledProtocols(protocols(socket.getSupportedProtocols()));socket.connect(new InetSocketAddress(address,port),3000);socket.startHandshake();
                in=new DataInputStream(socket.getInputStream());out=new DataOutputStream(socket.getOutputStream());
                if(cancelled.getAsBoolean())throw new IOException("Join cancelled");
                RoomWire.write(out,RoomWire.join(new RoomWire.Join(name,echo)));
                RoomWire.Packet first=RoomWire.decode(RoomWire.read(in));
                if(first.type==RoomWire.REJECT)throw new Rejected(RoomWire.parseReject(first));
                if(first.type!=RoomWire.ACCEPT)throw new IOException("Expected room acceptance");
                view=RoomWire.parseState(first);if(!MessageDigest.isEqual(view.nonce,echo))throw new IOException("Join nonce mismatch");
                socket.setSoTimeout(5000);
            }catch(IOException|RuntimeException e){closeSocket(socket);throw e;}
            finally{deadline.shutdownNow();}
            heartbeat=Executors.newSingleThreadScheduledExecutor(RoomTls::thread);
            reader=thread(this::receive);
            inputWriter=thread(this::writeInputs);
            heartbeat.scheduleAtFixedRate(this::ping,0,1000,TimeUnit.MILLISECONDS);
            reader.start();inputWriter.start();
        }
        public RoomWire.State snapshot(){RoomWire.State value=view;return value==null?null:RoomWire.copy(value);}
        public RoomStoryWire.Ticket storyTicket(){return story;}
        /** Host-origin, validated roster/epoch only. No game actor pointer. */
        public RoomWorldSnapshotWire.Snapshot worldSnapshot(){
            return lastWorldSnapshotAt>0&&now()-lastWorldSnapshotAt<=350?
                   worldSnapshot:null;
        }
        /** Current signed host NPCs. No guest game-engine mutation. */
        public RoomNpcWorldWire.World npcWorld(){
            return lastNpcAt>0&&now()-lastNpcAt<=350?npcWorld:null;
        }
        public boolean storyCommitted(){return story!=null&&storyCommitted;}
        /**
         * Must be called ONLY after this phone's actual BLACK game scene
         * loader confirms the host-selected mission/stage/epoch. Merely
         * joining a lobby never calls this and cannot start a solo mission.
         */
        public boolean acknowledgeSceneLoaded(int mission,int stage,int epoch)throws IOException{
            synchronized(lifecycle){
                RoomWire.State state=view;
                RoomStoryWire.Ticket ticket=story;
                if(closed.get()||ticket==null||storyReadySent||storyCommitted||
                   mission!=ticket.mission||stage!=ticket.stage||epoch!=ticket.sceneEpoch||
                   state==null||state.revision!=ticket.rosterRevision||
                   !MessageDigest.isEqual(state.session,ticket.session()))return false;
                int slot=0;
                for(int i=0;i<state.players.size();i++)
                    if(state.players.get(i).id==state.self){slot=i+1;break;}
                if(slot==0||!ticket.includes(slot)||slot==1)return false;
                // The packet is at most 36 bytes and encoded in memory. Never
                // hold the story or output locks while doing socket I/O here.
                final byte[] ready=RoomStoryWire.encode(RoomWire.STORY_READY,ticket);
                try{
                    readyWriter.execute(()->sendReadyFromNetworkWorker(ticket,ready));
                }catch(RejectedExecutionException unavailable){
                    record("STORY_GUEST_READY_QUEUE_FULL");
                    return false;
                }
                storyReadySent=true;
                record("STORY_GUEST_READY_QUEUED");
                return true; // queued, NOT evidence the host received READY
            }
        }
        private void sendReadyFromNetworkWorker(RoomStoryWire.Ticket expected,byte[] ready){
            // If the scene/roster was cancelled while waiting for the worker,
            // drop the stale ready before touching the socket.
            synchronized(lifecycle){
                RoomWire.State state=view;
                if(closed.get()||story==null||!story.same(expected)||
                   !storyReadySent||storyCommitted||state==null||
                   state.revision!=expected.rosterRevision)return;
            }
            try{
                synchronized(out){
                    if(closed.get())return;
                    RoomWire.write(out,ready);
                }
                record("STORY_GUEST_READY_SENT");
            }catch(IOException failure){
                this.failure="Connection lost";
                record("STORY_GUEST_READY_SEND_FAILED");
                close();
            }
        }
        /** Original native camera update, no socket I/O on caller thread. */
        public boolean offerOriginalCameraEye(int sequence,int x,int y,int z,int bx,int by,int bz)
            throws IOException{
            synchronized(lifecycle){
                RoomStoryWire.Ticket ticket=story;
                RoomWire.State state=view;
                if(closed.get()||!storyCommitted||ticket==null||state==null||
                   state.revision!=ticket.rosterRevision||
                   !MessageDigest.isEqual(state.session,ticket.session())||
                   sequence<=lastQueuedEyeSequence)return false;
                byte[] frame=RoomCameraEyeWire.encode(ticket,sequence,x,y,z,bx,by,bz);
                lastQueuedEyeSequence=sequence;
                cameraFrames.clear();
                return cameraFrames.offer(frame);
            }
        }
        /** Only the RoomSession verified game-thread adapter should call. */
        public boolean offerHumanInput(int seq,int x,int y,int yaw,
                                       boolean fire,boolean jump)throws IOException {
            synchronized(lifecycle){
                RoomStoryWire.Ticket ticket=story;
                RoomWire.State state=view;
                if(closed.get()||!storyCommitted||ticket==null||state==null||
                   state.revision!=ticket.rosterRevision||
                   !MessageDigest.isEqual(state.session,ticket.session())||
                   seq<=lastQueuedInputSequence)return false;
                byte[] frame=RoomHumanInputWire.encode(ticket,seq,x,y,yaw,fire,jump);
                lastQueuedInputSequence=seq;
                inputFrames.clear();
                return inputFrames.offer(frame);
            }
        }
        private void writeInputs(){
            try{
                while(!closed.get()){
                    // Camera gets priority at 10Hz; movement retains its own
                    // independent one-slot coalesced queue.
                    byte[] frame=cameraFrames.poll();
                    if(frame==null)frame=inputFrames.poll(10,TimeUnit.MILLISECONDS);
                    if(frame==null)continue;
                    RoomWire.Packet packet=RoomWire.decode(frame);
                    boolean camera=packet.type==RoomWire.HUMAN_CAMERA_EYE;
                    RoomStoryWire.Ticket ticket;
                    synchronized(lifecycle){
                        ticket=story;
                        if(closed.get()||!storyCommitted||ticket==null)continue;
                        if(camera){
                            if(!RoomCameraEyeWire.parse(packet).matches(ticket))continue;
                        }else if(packet.type==RoomWire.HUMAN_INPUT){
                            if(!RoomHumanInputWire.parse(packet).matches(ticket))continue;
                        }else throw new IOException("Invalid local movement packet");
                    }
                    synchronized(out){
                        if(closed.get())return;
                        RoomWire.write(out,frame);
                    }
                    if(camera){
                        if(firstCameraEyeSent.compareAndSet(false,true))
                            record("ORIGINAL_EYE_TLS_FIRST_SENT");
                    }else if(firstHumanInputSent.compareAndSet(false,true)){
                        record("HUMAN_INPUT_TLS_FIRST_SENT");
                    }
                }
            }catch(InterruptedException done){
                Thread.currentThread().interrupt();
            }catch(IOException|RuntimeException failure){
                record("HUMAN_INPUT_TLS_FAILED");
                close();
            }
        }
        public String failure(){return failure;}
        public boolean isRunning(){return !closed.get();}
        private void ping(){
            if(closed.get())return;
            try{
                if(now()-lastPong>5000)throw new IOException("Host heartbeat expired");
                if(sequence==Long.MAX_VALUE)throw new IOException("Sequence exhausted");
                synchronized(out){RoomWire.write(out,RoomWire.pulse(RoomWire.PING,++sequence));}
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
                    if(packet.type==RoomWire.STORY_PREPARE){
                        RoomStoryWire.Ticket incoming=RoomStoryWire.parse(packet);
                        RoomWire.State state=view;
                        if(state==null||story!=null||
                           incoming.rosterRevision!=state.revision||
                           !MessageDigest.isEqual(incoming.session(),state.session)||
                           incoming.participants!=(1<<state.players.size())-1)
                            throw new IOException("Story PREPARE for wrong room or roster");
                        int slot=0;
                        for(int i=0;i<state.players.size();i++)
                            if(state.players.get(i).id==state.self){slot=i+1;break;}
                        if(slot<=1||!incoming.includes(slot))
                            throw new IOException("Story PREPARE omits this guest");
                        synchronized(lifecycle){
                            if(!closed.get()){
                                story=incoming;storyReadySent=false;storyCommitted=false;
                                lastQueuedInputSequence=0;inputFrames.clear();firstHumanInputSent.set(false);
                                lastQueuedEyeSequence=0;cameraFrames.clear();firstCameraEyeSent.set(false);
                                worldSnapshot=null;lastWorldSnapshotTick=0;lastWorldSnapshotAt=0;
                                npcWorld=null;pendingNpc=null;lastNpcTick=0;lastNpcAt=0;
                                record("STORY_GUEST_PREPARED");
                            }
                        }
                        continue;
                    }
                    if(packet.type==RoomWire.STORY_COMMIT){
                        RoomStoryWire.Ticket incoming=RoomStoryWire.parse(packet);
                        synchronized(lifecycle){
                            if(story==null||!storyReadySent||storyCommitted||
                               !story.same(incoming))throw new IOException("Untrusted story COMMIT");
                            storyCommitted=true;
                            record("STORY_GUEST_COMMITTED");
                        }
                        continue;
                    }
                    if(packet.type==RoomWire.STORY_ABORT){
                        RoomStoryWire.Ticket incoming=RoomStoryWire.parse(packet);
                        synchronized(lifecycle){
                            if(story!=null&&story.same(incoming)){
                                story=null;storyReadySent=false;storyCommitted=false;
                                lastQueuedInputSequence=0;inputFrames.clear();firstHumanInputSent.set(false);
                                lastQueuedEyeSequence=0;cameraFrames.clear();firstCameraEyeSent.set(false);
                                worldSnapshot=null;lastWorldSnapshotTick=0;lastWorldSnapshotAt=0;
                                npcWorld=null;pendingNpc=null;lastNpcTick=0;lastNpcAt=0;
                                record("STORY_GUEST_ABORTED");
                            }
                        }
                        continue;
                    }
                    if(packet.type==RoomWire.WORLD_SNAPSHOT){
                        RoomWorldSnapshotWire.Snapshot sample=
                            RoomWorldSnapshotWire.parse(packet);
                        synchronized(lifecycle){
                            RoomWire.State current=view;
                            if(closed.get()||!storyCommitted||story==null||
                               current==null||current.revision!=story.rosterRevision||
                               !sample.matches(story)||sample.tick<=lastWorldSnapshotTick)
                                throw new IOException("Uncommitted or replayed host snapshot");
                            if(current.players.size()<2||current.players.size()>4||
                               RoomWire.storySlot(current,current.self)<2)
                                throw new IOException("Invalid snapshot roster");
                            int seen=0;
                            for(RoomWorldSnapshotWire.Avatar a:sample.avatars){
                                if(a.playerId<1||a.playerId>current.players.size()||
                                   !story.includes(a.playerId))
                                    throw new IOException("Snapshot contains unknown slot");
                                seen|=1<<(a.playerId-1);
                            }
                            if(seen!=story.participants)
                                throw new IOException("Incomplete snapshot roster");
                            lastWorldSnapshotTick=sample.tick;
                            worldSnapshot=sample;lastWorldSnapshotAt=now();
                        }
                        continue;
                    }
                    if(packet.type==RoomWire.NPC_WORLD){
                        RoomNpcWorldWire.Chunk part=RoomNpcWorldWire.parse(packet);
                        synchronized(lifecycle){
                            RoomWire.State current=view;
                            if(closed.get()||!storyCommitted||story==null||
                               current==null||
                               current.revision!=story.rosterRevision||
                               !part.matches(story))
                                throw new IOException("Untrusted original host NPC source");
                            if(part.tick<=lastNpcTick)continue;
                            if(part.part==0){
                                pendingNpc=part;
                                if(part.parts==1){
                                    npcWorld=RoomNpcWorldWire.complete(part,null);
                                    lastNpcTick=part.tick;lastNpcAt=now();pendingNpc=null;
                                }
                            }else{
                                if(pendingNpc==null||pendingNpc.tick!=part.tick||
                                   pendingNpc.parts!=2){pendingNpc=null;continue;}
                                npcWorld=RoomNpcWorldWire.complete(pendingNpc,part);
                                lastNpcTick=part.tick;lastNpcAt=now();pendingNpc=null;
                            }
                        }
                        continue;
                    }
                    if(packet.type!=RoomWire.ROSTER)throw new IOException("Unexpected host message");
                    RoomWire.State next=RoomWire.parseState(packet),old=view;
                    if(old==null||next.self!=old.self||!MessageDigest.isEqual(next.session,old.session)||
                       !MessageDigest.isEqual(next.nonce,old.nonce)||next.revision<old.revision||!next.room.equals(old.room))throw new IOException("Room identity changed");
                    publish(next);
                    synchronized(lifecycle){
                        if(story!=null&&story.rosterRevision!=next.revision){
                            story=null;storyReadySent=false;storyCommitted=false;
                            lastQueuedInputSequence=0;inputFrames.clear();firstHumanInputSent.set(false);
                                lastQueuedEyeSequence=0;cameraFrames.clear();firstCameraEyeSent.set(false);
                                worldSnapshot=null;lastWorldSnapshotTick=0;lastWorldSnapshotAt=0;
                                npcWorld=null;pendingNpc=null;lastNpcTick=0;lastNpcAt=0;
                            record("STORY_GUEST_ROSTER_INVALIDATED");
                        }
                    }
                }
            }catch(IOException|RuntimeException e){if(!closed.get())failure="Disconnected";}
            finally{close();}
        }
        public void close(){
            synchronized(lifecycle){
                if(!closed.compareAndSet(false,true))return;
                view=null;story=null;storyReadySent=false;storyCommitted=false;
                lastQueuedInputSequence=0;inputFrames.clear();firstHumanInputSent.set(false);
                                lastQueuedEyeSequence=0;cameraFrames.clear();firstCameraEyeSent.set(false);
                                worldSnapshot=null;lastWorldSnapshotTick=0;lastWorldSnapshotAt=0;
                                npcWorld=null;pendingNpc=null;lastNpcTick=0;lastNpcAt=0;
            }
            readyWriter.shutdownNow();
            closeSocket(socket);heartbeat.shutdownNow();reader.interrupt();inputWriter.interrupt();
        }
    }
}
