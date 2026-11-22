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
        // SESSION69: synthetic native core/stream MATCH enters the genuine
        // RoomSession original-game-thread facade. The Java source gate may
        // send TLS STORY_PREPARE, but never scene READY/COMMIT or a guest loader.
        try(RoomSession owner=RoomSession.create();RoomSession remote=RoomSession.create()){
            owner.startHost(source,loop,0,"Native host","Native stage");
            until(()->owner.snapshot().phase==RoomSession.Phase.HOSTING);
            RoomSession.Status access=owner.snapshot();
            remote.join("127.0.0.1:"+access.port,access.fingerprint,"Rejoin guest");
            until(()->remote.snapshot().phase==RoomSession.Phase.JOINED&&
                      owner.snapshot().roster.players.size()==2);
            check(!owner.prepareObservedHostStageFromGameThread()); // no source
            int[] beforeTicket=remote.guestPreparedStoryFromOriginalFrontend();
            check(beforeTicket.length==8&&java.util.Arrays.stream(beforeTicket).allMatch(x->x==0));
            check(owner.bindStoryGameThread()); // fake callback owner thread
            check(owner.observeOriginalStageIdentityFromGameThread(11,0,0));
            check(owner.prepareObservedHostStageFromGameThread());
            until(()->remote.storyStatus().ticket!=null);
            RoomStoryWire.Ticket original=owner.storyStatus().ticket;
            check(original!=null&&original.mission==0&&original.stage==0&&
                  original.sceneEpoch==1&&original.participants==3);
            check(!owner.storyStatus().committed&&!remote.storyStatus().committed);
            int[] originalGuestFrontend=remote.guestPreparedStoryFromOriginalFrontend();
            check(originalGuestFrontend.length==8&&originalGuestFrontend[0]==1&&
                  originalGuestFrontend[1]==0&&originalGuestFrontend[2]==0&&
                  originalGuestFrontend[6]==2&&originalGuestFrontend[7]==3);
            long token=((long)originalGuestFrontend[4]<<32)|
                       (originalGuestFrontend[5]&0xffffffffL);
            check(token==original.token&&originalGuestFrontend[3]==(int)original.rosterRevision);
            java.util.concurrent.atomic.AtomicBoolean foreignAccepted=
                new java.util.concurrent.atomic.AtomicBoolean();
            Thread unrelated=new Thread(()->{
                int[] untrusted=remote.guestPreparedStoryFromOriginalFrontend();
                foreignAccepted.set(java.util.Arrays.stream(untrusted).anyMatch(x->x!=0));
            },"untrusted-non-original-frontend");
            unrelated.start();unrelated.join();
            check(!foreignAccepted.get()); // same original frontend thread only
            check(!owner.prepareObservedHostStageFromGameThread());
            check(owner.cancelStoryFromGameThread());
            until(()->remote.storyStatus().ticket==null);
            check(java.util.Arrays.stream(remote.guestPreparedStoryFromOriginalFrontend())
                .allMatch(x->x==0)); // no stale stage after abort
            check(!owner.observeOriginalStageIdentityFromGameThread(11,0,0));
            check(!owner.prepareObservedHostStageFromGameThread()); // same roster
            int priorWireId=remote.snapshot().roster.self;
            remote.stop();
            until(()->owner.snapshot().roster.players.size()==1);
            remote.join("127.0.0.1:"+access.port,access.fingerprint,"Rejoined human");
            until(()->remote.snapshot().phase==RoomSession.Phase.JOINED&&
                      owner.snapshot().roster.players.size()==2);
            check(remote.snapshot().roster.self>priorWireId);
            check(!owner.observeOriginalStageIdentityFromGameThread(11,0,0));
            check(owner.prepareObservedHostStageFromGameThread());
            until(()->remote.storyStatus().ticket!=null);
            RoomStoryWire.Ticket after=owner.storyStatus().ticket;
            check(after!=null&&after.sceneEpoch==2&&
                  after.mission==0&&after.stage==0&&after.participants==3&&
                  !after.same(original));
            check(!remote.storyStatus().committed);
            int[] secondGuestPlan=remote.guestPreparedStoryFromOriginalFrontend();
            check(secondGuestPlan[0]==2&&secondGuestPlan[1]==0&&
                  secondGuestPlan[6]==2&&secondGuestPlan[7]==3);
            // Host original stage transition must revoke old PREPARE and
            // announce the newer source-matched original campaign stage.
            check(owner.observeOriginalStageIdentityFromGameThread(12,1,0));
            check(owner.prepareObservedHostStageFromGameThread());
            until(()->remote.storyStatus().ticket!=null&&
                remote.storyStatus().ticket.mission==1);
            RoomStoryWire.Ticket advanced=owner.storyStatus().ticket;
            check(advanced!=null&&advanced.sceneEpoch==3&&advanced.mission==1&&
                  advanced.stage==0&&!advanced.same(after));
            check(!owner.storyStatus().committed&&!remote.storyStatus().committed);
            int[] nextGuestPlan=remote.guestPreparedStoryFromOriginalFrontend();
            check(nextGuestPlan[0]==3&&nextGuestPlan[1]==1&&
                  nextGuestPlan[2]==0&&nextGuestPlan[6]==2&&nextGuestPlan[7]==3);
            remote.stop();
            check(java.util.Arrays.stream(remote.guestPreparedStoryFromOriginalFrontend())
                .allMatch(x->x==0)); // network disconnect resets frontend signal
        }
        // SESSION73: real loopback TLS plus SYNTHETIC simulation of verified
        // ORIGINAL running world callback. No game runtime in JVM; source
        // proof is independently covered in the C++ 30-sample gate test.
        try(RoomSession hostReady=RoomSession.create();
            RoomSession guestReady=RoomSession.create()){
            hostReady.startHost(source,loop,0,"Source host","Source stage");
            until(()->hostReady.snapshot().phase==RoomSession.Phase.HOSTING);
            RoomSession.Status h=hostReady.snapshot();
            guestReady.join("127.0.0.1:"+h.port,h.fingerprint,"Stage guest");
            until(()->guestReady.snapshot().phase==RoomSession.Phase.JOINED&&
                     hostReady.snapshot().roster.players.size()==2);
            check(hostReady.bindStoryGameThread()&&guestReady.bindStoryGameThread());
            check(hostReady.attestOriginalRunningSceneFromGameThread(
                23,0,0,0,0,0,0,0)==0); // no real original stage
            check(hostReady.observeOriginalStageIdentityFromGameThread(23,0,0));
            check(hostReady.prepareObservedHostStageFromGameThread());
            until(()->guestReady.storyStatus().ticket!=null);
            RoomStoryWire.Ticket ready=hostReady.storyStatus().ticket;
            check(ready!=null&&ready.mission==0&&ready.stage==0);
            check(hostReady.attestOriginalRunningSceneFromGameThread(
                23,1,0,0,0,0,0,0)==0);
            check(hostReady.attestOriginalRunningSceneFromGameThread(
                24,0,0,0,0,0,0,0)==0);
            check(guestReady.attestOriginalRunningSceneFromGameThread(
                16,0,0,ready.sceneEpoch,(int)ready.rosterRevision,
                (int)(ready.token>>>32),(int)ready.token,2)==0);
            check(hostReady.attestOriginalRunningSceneFromGameThread(
                23,0,0,0,0,0,0,0)==1);
            check(hostReady.attestOriginalRunningSceneFromGameThread(
                23,0,0,0,0,0,0,0)==0);
            check(!hostReady.storyStatus().committed);
            check(guestReady.observeOriginalStageIdentityFromGameThread(16,0,0));
            check(guestReady.attestOriginalRunningSceneFromGameThread(
                16,0,0,ready.sceneEpoch,(int)ready.rosterRevision,
                (int)(ready.token>>>32),(int)ready.token+1,2)==0);
            check(guestReady.attestOriginalRunningSceneFromGameThread(
                16,0,0,ready.sceneEpoch,(int)ready.rosterRevision,
                (int)(ready.token>>>32),(int)ready.token,3)==0);
            check(guestReady.attestOriginalRunningSceneFromGameThread(
                16,0,0,ready.sceneEpoch,(int)ready.rosterRevision,
                (int)(ready.token>>>32),(int)ready.token,2)==2);
            check(guestReady.attestOriginalRunningSceneFromGameThread(
                16,0,0,ready.sceneEpoch,(int)ready.rosterRevision,
                (int)(ready.token>>>32),(int)ready.token,2)==0);
            until(()->hostReady.storyStatus().committed&&
                     guestReady.storyStatus().committed);
            // The REAL TLS barrier is now complete. Verify authoritative
            // host world distribution and authenticated guest->host human
            // command, using synthetic source positions only.
            java.util.List<RoomWorldSnapshotWire.Avatar> world=
                java.util.Arrays.asList(
                  new RoomWorldSnapshotWire.Avatar(1,250,400,500,0,0),
                  new RoomWorldSnapshotWire.Avatar(2,325,400,500,0,0));
            check(hostReady.publishWorldSnapshotFromGameThread(1,world));
            until(()->guestReady.readHostWorldFromGameThread()!=null&&
                guestReady.readHostWorldFromGameThread().tick==1);
            check(guestReady.readHostWorldFromGameThread().avatars.size()==2);
            check(guestReady.offerHumanInputFromGameThread(
                1,500,-200,4000,true,false));
            java.util.concurrent.atomic.AtomicBoolean acceptedGuest=
                new java.util.concurrent.atomic.AtomicBoolean();
            until(()->{
                for(RoomTls.Server.TrustedInput command:
                    hostReady.drainHumanInputsFromGameThread())
                    if(command.playerSlot==2&&command.value.fire&&
                       command.value.moveX==500)acceptedGuest.set(true);
                return acceptedGuest.get();
            });
            check(hostReady.cancelStoryFromGameThread());
            until(()->guestReady.storyStatus().ticket==null);
            check(!guestReady.storyStatus().committed&&
                  guestReady.readHostWorldFromGameThread()==null);
            check(!guestReady.offerHumanInputFromGameThread(
                2,0,0,0,false,false));
        }
        System.out.println("ORIGINAL_STABLE_RUNNING_STAGE_TLS_READY_GATE_PASS: synthetic callback, real TLS ticket+roster+original source identity; no auto guest loader or shared AI");
        System.out.println("STORY_DIAGNOSTICS_PASS: authenticated TLS lifecycle emits only bounded safe codes even without room UI");
        System.out.println("SESSION_STORY_BRIDGE_PASS: authenticated TLS ticket via pinned game-thread facade, no implicit READY, host-only prepare, replay and cancel fences");
        System.out.println("ROOM_SESSION_PASS: real host/join, immutable roster, reject, cancellation, lifecycle");
    }
}
