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
        // Session-level GAME-THREAD handoff across real, pinned TLS sockets.
        // These calls simulate a future verified engine callback; no BLACK
        // level, game-thread hook or remote actor runs in this JVM test.
        try(RoomSession owner=RoomSession.create();RoomSession remote=RoomSession.create()){
            java.util.concurrent.CopyOnWriteArrayList<String> hostEvents=new java.util.concurrent.CopyOnWriteArrayList<>();
            java.util.concurrent.CopyOnWriteArrayList<String> guestEvents=new java.util.concurrent.CopyOnWriteArrayList<>();
            owner.setReporter(hostEvents::add);
            remote.setReporter(guestEvents::add);
            owner.startHost(source,loop,0,"Story host","Story room");
            until(()->owner.snapshot().phase==RoomSession.Phase.HOSTING);
            RoomSession.Status endpoint=owner.snapshot();
            remote.join("127.0.0.1:"+endpoint.port,endpoint.fingerprint,"Story guest");
            until(()->remote.snapshot().phase==RoomSession.Phase.JOINED);
            until(()->owner.snapshot().roster.players.size()==2);
            check(owner.storyStatus().ticket==null&&!owner.storyStatus().committed);
            check(remote.storyStatus().ticket==null&&!remote.storyStatus().committed);
            // Merely joining cannot acquire game-thread proof or start a mission.
            try{owner.prepareStoryFromGameThread(0,0,17);throw new AssertionError("Unbound host was accepted");}
            catch(java.io.IOException expected){}
            check(!remote.reportSceneLoadedFromGameThread(0,0,17));
            check(owner.bindStoryGameThread()&&remote.bindStoryGameThread());
            java.util.concurrent.atomic.AtomicBoolean crossThreadAccepted=new java.util.concurrent.atomic.AtomicBoolean(false);
            Thread networkWorker=new Thread(()->{
                try{
                    if(owner.bindStoryGameThread()||remote.reportSceneLoadedFromGameThread(0,0,17))
                        crossThreadAccepted.set(true);
                }catch(java.io.IOException e){throw new AssertionError(e);}
            },"synthetic-not-game-thread");
            networkWorker.start();networkWorker.join();
            check(!crossThreadAccepted.get());
            RoomStoryWire.Ticket first=owner.prepareStoryFromGameThread(0,0,17);
            until(()->remote.storyStatus().ticket!=null);
            check(first.same(remote.storyStatus().ticket));
            check(!remote.reportSceneLoadedFromGameThread(0,0,18)); // wrong scene
            check(!owner.reportSceneLoadedFromGameThread(0,1,17)); // wrong stage
            check(!owner.reportSceneLoadedFromGameThread(0,0,17)); // waits for guest
            check(!owner.storyStatus().committed);
            check(remote.reportSceneLoadedFromGameThread(0,0,17)); // hypothetical real load
            until(()->owner.storyStatus().committed&&remote.storyStatus().committed);
            until(()->hostEvents.contains("STORY_HOST_PREPARED")&&
                     hostEvents.contains("STORY_HOST_SCENE_READY")&&
                     hostEvents.contains("STORY_HOST_PEER_READY")&&
                     hostEvents.contains("STORY_HOST_COMMITTED")&&
                     guestEvents.contains("STORY_GUEST_PREPARED")&&
                     guestEvents.contains("STORY_GUEST_READY_SENT")&&
                     guestEvents.contains("STORY_GUEST_COMMITTED"));
            check(!remote.reportSceneLoadedFromGameThread(0,0,17)); // no duplicate READY
            check(owner.cancelStoryFromGameThread());
            until(()->remote.storyStatus().ticket==null);
            until(()->hostEvents.contains("STORY_HOST_ABORTED")&&
                     guestEvents.contains("STORY_GUEST_ABORTED"));
            check(!remote.storyStatus().committed);
            RoomStoryWire.Ticket second=owner.prepareStoryFromGameThread(1,0,18);
            check(second.token!=first.token);
            until(()->remote.storyStatus().ticket!=null);
            check(owner.cancelStoryFromGameThread());
            until(()->remote.storyStatus().ticket==null);
            remote.stop();
            check(!remote.bindStoryGameThread());
            check(!remote.reportSceneLoadedFromGameThread(1,0,18));
            for(String code:hostEvents)check(code.matches("[A-Z0-9_]{1,70}")&&
                !code.contains("Story room")&&!code.contains("127.0.0.1"));
            for(String code:guestEvents)check(code.matches("[A-Z0-9_]{1,70}")&&
                !code.contains("Story guest")&&!code.contains("127.0.0.1"));
        }
        System.out.println("STORY_DIAGNOSTICS_PASS: authenticated TLS lifecycle emits only bounded safe codes even without room UI");
        System.out.println("SESSION_STORY_BRIDGE_PASS: authenticated TLS ticket via pinned game-thread facade, no implicit READY, host-only prepare, replay and cancel fences");
        System.out.println("ROOM_SESSION_PASS: real host/join, immutable roster, reject, cancellation, lifecycle");
    }
}
