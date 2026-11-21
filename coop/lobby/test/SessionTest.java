// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.net.InetAddress;
import java.nio.file.Paths;
import javax.net.ssl.SSLContext;

/** Real JVM socket test of asynchronous host/join ownership and stale requests. */
public final class SessionTest {
    private interface Check { boolean valid() throws Exception; }
    private static void check(boolean okay){if(!okay)throw new AssertionError("RoomSession invariant");}
    private static void until(Check assertion)throws Exception {
        long end=System.nanoTime()+8_000_000_000L;
        while(!assertion.valid()){
            if(System.nanoTime()>end)throw new AssertionError("Timed out waiting for room lifecycle");
            Thread.sleep(20);
        }
    }
    public static void main(String[]args)throws Exception{
        SSLContext server=TlsRoomTest.host(Paths.get(args[0]));
        byte[] pin=TlsRoomTest.fingerprint(Paths.get(args[0]));
        RoomSession.IdentitySource source=()->new RoomSession.HostIdentity(server,pin);
        InetAddress loop=InetAddress.getLoopbackAddress();
        try(RoomSession hosting=RoomSession.create();RoomSession joining=RoomSession.create()){
            hosting.startHost(source,loop,0,"Phone host","BLACK coop");
            until(()->hosting.snapshot().phase==RoomSession.Phase.HOSTING);
            RoomSession.Status start=hosting.snapshot();
            check(start.port>0&&start.fingerprint.length()==64&&start.roster.players.size()==1);
            String address="127.0.0.1:"+start.port;
            joining.join(address,start.fingerprint,"Phone client");
            until(()->joining.snapshot().phase==RoomSession.Phase.JOINED);
            until(()->hosting.snapshot().roster.players.size()==2&&joining.snapshot().roster.players.size()==2);
            byte[] stolen=joining.snapshot().roster.session;
            stolen[0]^=42;
            check(joining.snapshot().roster.session[0]!=stolen[0]);
            joining.stop();
            until(()->hosting.snapshot().roster.players.size()==1);
            // Rejected stale join may finish later, but cannot override a newer stop.
            joining.join(address,start.fingerprint,"Cancelled");
            joining.stop();
            check(joining.snapshot().phase==RoomSession.Phase.IDLE);
            hosting.stop();
            check(hosting.snapshot().phase==RoomSession.Phase.IDLE);
            joining.join(address,start.fingerprint,"After shutdown");
            until(()->joining.snapshot().phase==RoomSession.Phase.ERROR);
        }
        // A stopped queued request must never reach the host, even after the worker unblocks.
        java.util.concurrent.CountDownLatch entered=new java.util.concurrent.CountDownLatch(1),release=new java.util.concurrent.CountDownLatch(1);
        try(RoomTls.Server host=RoomTls.Server.start(server,loop,0,"Host","Cancel");RoomSession queued=RoomSession.create()){
            queued.startHost(()->{entered.countDown();try{release.await();}catch(InterruptedException e){Thread.currentThread().interrupt();}return new RoomSession.HostIdentity(server,pin);},loop,0,"Blocked","Blocked");
            check(entered.await(2,java.util.concurrent.TimeUnit.SECONDS));
            queued.joinAutomatic("127.0.0.1:"+host.port(),"Cancelled queued guest");
            queued.stop();release.countDown();Thread.sleep(500);
            check(host.snapshot().players.size()==1);
            try(RoomTls.Client next=RoomDirect.joinAutomatic("127.0.0.1:"+host.port(),"Actual guest")){
                check(next.snapshot().self==2); // no ID consumed by the cancelled queued join
            }
        }finally{release.countDown();}
        System.out.println("ROOM_SESSION_PASS: real host/join, immutable roster, reject, cancellation, lifecycle");
    }
}
