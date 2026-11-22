// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.IOException;
import java.net.ConnectException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import javax.net.ssl.SSLHandshakeException;
import java.net.InetAddress;
import java.security.GeneralSecurityException;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Executor;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;

/**
 * Explicit, bounded owner-controlled lobby lifecycle. The worker never calls
 * the game, modifies guest memory or creates simulated players.
 */
public final class RoomSession implements AutoCloseable {
    public enum Phase { IDLE, CONNECTING, HOSTING, JOINED, ERROR }
    public static final class HostIdentity {
        public final SSLContext tls;
        public final byte[] fingerprint;
        public HostIdentity(SSLContext tls, byte[] fingerprint) {
            if(tls==null || fingerprint==null || fingerprint.length!=32)
                throw new IllegalArgumentException("Full host identity is required");
            this.tls=tls;this.fingerprint=fingerprint.clone();
        }
    }
    public interface IdentitySource { HostIdentity load() throws Exception; }
    public interface Reporter { void event(String safeCode); }
    private volatile Reporter reporter;
    public void setReporter(Reporter sink){reporter=sink;}
    private void event(String code){Reporter sink=reporter;if(sink!=null)try{sink.event(code);}catch(RuntimeException ignored){}}
    public static final class Status {
        public final Phase phase;
        public final String description, fingerprint;
        public final int port;
        public final RoomWire.State roster;
        private Status(Phase phase,String description,String fingerprint,int port,RoomWire.State roster) {
            this.phase=phase;this.description=description;this.fingerprint=fingerprint;
            this.port=port;this.roster=roster==null?null:RoomWire.copy(roster);
        }
    }
    private final Object lock=new Object();
    private final ThreadPoolExecutor work=new ThreadPoolExecutor(
        1,1,0L,TimeUnit.MILLISECONDS,new ArrayBlockingQueue<>(1),r->{
            Thread t=new Thread(r,"bo-lobby-session");t.setDaemon(true);return t;
        },new ThreadPoolExecutor.DiscardOldestPolicy());
    // Only a future VERIFIED BLACK engine callback may claim this thread.
    // This Java guard prevents socket/UI workers from accidentally invoking
    // the story APIs once the real game thread is bound. It is not proof of
    // a real game hook, scene loader, or shared simulation.
    private Thread storyGameThread;
    private long generation;
    private boolean closed;

    /** Read-only ticket status; no live socket or game actor is exposed. */
    public static final class StoryStatus {
        public final RoomStoryWire.Ticket ticket;
        public final boolean committed;
        private StoryStatus(RoomStoryWire.Ticket ticket,boolean committed){
            this.ticket=ticket;this.committed=committed;
        }
    }

    /**
     * Called exclusively from a future verified BLACK game-thread callback,
     * never from an Android button or a TLS reader. A new room must rebind.
     * This is a thread-identity guard, NOT a scene-load attestation.
     */
    public boolean bindStoryGameThread(){
        synchronized(lock){
            if(closed||(phase!=Phase.HOSTING&&phase!=Phase.JOINED))return false;
            if(storyGameThread==null)storyGameThread=Thread.currentThread();
            return storyGameThread==Thread.currentThread();
        }
    }
    private boolean validStoryThread(){
        return !closed&&storyGameThread!=null&&storyGameThread==Thread.currentThread();
    }
    /** Never auto-starts a level or publishes READY when a guest joins. */
    public StoryStatus storyStatus(){
        synchronized(lock){
            if(host!=null&&phase==Phase.HOSTING)
                return new StoryStatus(host.storyTicket(),host.storyCommitted());
            if(client!=null&&phase==Phase.JOINED&&client.isRunning())
                return new StoryStatus(client.storyTicket(),client.storyCommitted());
            return new StoryStatus(null,false);
        }
    }
    /**
     * Mission ID 0..7 from an independently verified game-thread selection
     * only. No UI or network-thread direct access; host is sole authority.
     */
    public RoomStoryWire.Ticket prepareStoryFromGameThread(int mission,int stage,int epoch)
        throws IOException {
        synchronized(lock){
            if(!validStoryThread()||phase!=Phase.HOSTING||host==null)
                throw new IOException("No bound host game thread");
            return host.prepareStory(mission,stage,epoch);
        }
    }
    /**
     * The caller MUST independently prove the actual BLACK scene is loaded.
     * Host return means all participants committed; guest return means READY
     * was QUEUED for its separate TLS network writer (NOT received by host).
     * Neither result, alone, proves real BLACK scene load; only an independently
     * verified game-thread loader callback may call this adapter.
     */
    public boolean reportSceneLoadedFromGameThread(int mission,int stage,int epoch)
        throws IOException {
        synchronized(lock){
            if(!validStoryThread())return false;
            if(host!=null&&phase==Phase.HOSTING)
                return host.hostSceneLoaded(mission,stage,epoch);
            if(client!=null&&phase==Phase.JOINED&&client.isRunning())
                return client.acknowledgeSceneLoaded(mission,stage,epoch);
            return false;
        }
    }
    /** Real TLS movement command, offered only from the verified BLACK game thread. */
    public boolean offerHumanInputFromGameThread(int seq,int x,int y,int yaw,
                                                  boolean fire,boolean jump)
        throws IOException{
        synchronized(lock){
            return validStoryThread()&&phase==Phase.JOINED&&client!=null&&
                client.offerHumanInput(seq,x,y,yaw,fire,jump);
        }
    }
    /** Trust comes from TLS peer identity; no direct NPC or guest memory writes. */
    public java.util.List<RoomTls.Server.TrustedInput> drainHumanInputsFromGameThread(){
        synchronized(lock){
            if(!validStoryThread()||phase!=Phase.HOSTING||host==null)
                return java.util.Collections.emptyList();
            return host.drainLatestTrustedInputs();
        }
    }
    /** Host scene unload/cancel invalidates its prepared ticket. */
    public boolean cancelStoryFromGameThread(){
        synchronized(lock){
            return validStoryThread()&&phase==Phase.HOSTING&&host!=null&&
                   host.cancelStory();
        }
    }
    private Phase phase=Phase.IDLE;
    private String description="",fingerprint="";
    private RoomTls.Server host;
    private RoomDiscovery.Host announcer;
    private RoomTls.Client client;
    private RoomSession(){}
    public static RoomSession create(){return new RoomSession();}
    private static void closeQuietly(AutoCloseable resource){
        if(resource!=null)try{resource.close();}catch(Exception ignored){}
    }
    private long begin() {
        RoomTls.Server oldHost;
        RoomDiscovery.Host oldAnnouncer;
        RoomTls.Client oldClient;
        long token;
        synchronized(lock) {
            if(closed)throw new IllegalStateException("RoomSession was destroyed");
            token=++generation;
            storyGameThread=null;
            oldHost=host;oldClient=client;oldAnnouncer=announcer;
            host=null;client=null;announcer=null;phase=Phase.CONNECTING;
            description="Connecting";fingerprint="";
        }
        closeQuietly(oldAnnouncer);closeQuietly(oldClient);closeQuietly(oldHost);
        event("SESSION_CONNECTING");
        return token;
    }
    private boolean current(long token) {
        synchronized(lock){return !closed && generation==token;}
    }
    private void failure(long token, Exception cause) {
        String message="Could not establish room";
        if(cause instanceof RoomTls.Rejected) {
            int reason=((RoomTls.Rejected)cause).reason;
            message=reason==2?"Incompatible game version":
                    reason==3?"Room is full":reason==5?"Too many failed attempts":"Room rejected";
        } else if(cause instanceof SSLHandshakeException) {
            message="Automatic connection failed; check that both phones use the same game build";
        } else if(cause instanceof GeneralSecurityException) {
            message="Could not prepare the automatic connection";
        } else if(cause instanceof SocketTimeoutException) {
            message="Network timeout: check Wi-Fi and access-point isolation";
        } else if(cause instanceof ConnectException) {
            message="Host unreachable: check Wi-Fi or VPN peer connectivity";
        } else if(cause instanceof UnknownHostException) {
            message="Invalid or unreachable host address";
        } else if(cause instanceof IOException) {
            message="Network connection closed or unavailable";
        }
        boolean published=false;
        synchronized(lock) {
            if(!closed && generation==token) {
                phase=Phase.ERROR;description=message;fingerprint="";published=true;
            }
        }
        if(published)event("ERROR_"+cause.getClass().getSimpleName()
            .replaceAll("[^A-Za-z0-9]","").toUpperCase(java.util.Locale.ROOT));
    }
    private void submit(Runnable action,long token){
        try{work.execute(action);}
        catch(java.util.concurrent.RejectedExecutionException e){failure(token,e);}
    }
    public void startHost(IdentitySource identity,InetAddress bind,int port,
                          String displayName,String roomName) {
        if(identity==null||bind==null)throw new IllegalArgumentException("Missing host settings");
        long token=begin();
        submit(()->{
            RoomTls.Server candidate=null;
            RoomDiscovery.Host broadcast=null;
            try {
                if(!current(token))return;
                // Port 0 is reserved for localhost-only isolated instrumentation.
                if(port>65535 || (port<1024 && !(port==0 && bind.isLoopbackAddress())))
                    throw new IOException("Invalid listener port");
                HostIdentity key=identity.load();
                if(!current(token))return;
                candidate=RoomTls.Server.start(key.tls,bind,port,displayName,roomName);
                candidate.setSafeEventSink(this::event);
                boolean discoverable=true;
                if(!bind.isLoopbackAddress()&&port!=0){
                    try{broadcast=RoomDiscovery.Host.start(candidate,bind,RoomDiscovery.PORT,key.fingerprint);}
                    catch(IOException unavailable){discoverable=false;}
                }
                boolean owned;
                synchronized(lock){
                    owned=!closed&&generation==token;
                    if(owned) {
                        host=candidate;announcer=broadcast;phase=Phase.HOSTING;
                        description=discoverable?"Hosting":"Hosting (LAN discovery unavailable)";
                        StringBuilder hex=new StringBuilder(64);
                        for(byte b:key.fingerprint)hex.append(String.format(java.util.Locale.ROOT,"%02X",b&255));
                        fingerprint=hex.toString();
                    }
                }
                if(!owned){closeQuietly(broadcast);closeQuietly(candidate);}
                else event(discoverable?"HOST_DISCOVERY_READY":"HOST_DISCOVERY_DISABLED");
            } catch(Exception e) {
                closeQuietly(broadcast);closeQuietly(candidate);
                failure(token,e);
            }
        },token);
    }
    public void join(String ipv4Port,String hostFingerprint,String displayName) {
        long token=begin();
        submit(()->{
            RoomTls.Client candidate=null;
            try {
                if(!current(token))return;
                if(hostFingerprint==null)candidate=RoomDirect.joinAutomatic(ipv4Port,displayName,()->!current(token));
                else {
                    RoomDirect.Endpoint endpoint=RoomDirect.parse(ipv4Port);
                    candidate=RoomTls.Client.connect(RoomTls.pinnedContext(RoomDirect.fingerprint(hostFingerprint)),endpoint.address,endpoint.port,displayName,()->!current(token));
                }
                // Always-on small safe codes, including after dismissing room UI.
                candidate.setSafeEventSink(this::event);
                boolean owned;
                synchronized(lock){
                    owned=!closed&&generation==token;
                    if(owned) {
                        client=candidate;phase=Phase.JOINED;description="Joined";
                    }
                }
                if(!owned)closeQuietly(candidate);
                else event("JOIN_CONNECTED");
            } catch(Exception e) {
                closeQuietly(candidate);
                failure(token,e);
            }
        },token);
    }
    public void joinAutomatic(String target,String name){join(target,null,name);}
    /** An immutable-by-copy view; no socket or peer pointer reaches UI/game code. */
    public Status snapshot(){
        synchronized(lock) {
            RoomWire.State roster=null;
            int port=0;
            if(host!=null) {
                port=host.port();
                try{roster=host.snapshot();}
                catch(IOException e){return new Status(Phase.ERROR,"Room unavailable","",0,null);}
            } else if(client!=null) {
                roster=client.snapshot();
                if(!client.isRunning()||roster==null)
                    return new Status(Phase.ERROR,"Disconnected","",0,null);
            }
            return new Status(phase,description,fingerprint,port,roster);
        }
    }
    public void stop(){
        RoomTls.Server oldHost;
        RoomDiscovery.Host oldAnnouncer;
        RoomTls.Client oldClient;
        synchronized(lock){
            ++generation;storyGameThread=null;
            oldHost=host;oldClient=client;oldAnnouncer=announcer;
            host=null;client=null;announcer=null;phase=Phase.IDLE;description="";fingerprint="";
        }
        closeQuietly(oldAnnouncer);closeQuietly(oldClient);closeQuietly(oldHost);
        event("SESSION_STOPPED");
    }
    @Override public void close(){
        synchronized(lock){if(closed)return;closed=true;}
        stop();
        work.shutdownNow();
    }
}
