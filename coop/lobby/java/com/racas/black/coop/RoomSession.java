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
    private long generation;
    private boolean closed;
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
            ++generation;oldHost=host;oldClient=client;oldAnnouncer=announcer;
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
