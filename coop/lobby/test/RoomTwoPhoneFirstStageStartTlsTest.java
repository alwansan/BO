// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;
/** Real TLS two-phone START from lobby, with SYNTHETIC native scene events. */
public final class RoomTwoPhoneFirstStageStartTlsTest {
    private interface Check{boolean okay()throws Exception;}
    private static void ok(boolean yes){if(!yes)throw new AssertionError("Two-phone first story START");}
    private static void until(Check c)throws Exception{
        long deadline=System.nanoTime()+TimeUnit.SECONDS.toNanos(12);
        while(!c.okay()){
            if(System.nanoTime()>deadline)throw new AssertionError("Two phone story timeout");
            Thread.sleep(15);
        }
    }
    public static void main(String[]args)throws Exception{
        SSLContext tls=TlsRoomTest.host(Paths.get(args[0]));
        byte[] fingerprint=TlsRoomTest.fingerprint(Paths.get(args[0]));
        RoomSession.IdentitySource identity=()->
            new RoomSession.HostIdentity(tls,fingerprint);
        try(RoomSession host=RoomSession.create();
            RoomSession guest=RoomSession.create()){
            host.startHost(identity,InetAddress.getByName("127.0.0.1"),0,
                "Phone host","FIRST STAGE");
            until(()->host.snapshot().phase==RoomSession.Phase.HOSTING);
            ok(!host.requestTwoPhoneFirstStageFromLobby()); // no second player
            RoomSession.Status hs=host.snapshot();
            guest.join("127.0.0.1:"+hs.port,hs.fingerprint,"Phone 2");
            until(()->guest.snapshot().phase==RoomSession.Phase.JOINED&&
                host.snapshot().roster.players.size()==2&&
                guest.snapshot().roster.players.size()==2);
            ok(host.requestTwoPhoneFirstStageFromLobby()); // UI thread
            ok(!host.requestTwoPhoneFirstStageFromLobby()); // no second START
            RoomStoryWire.Ticket first=host.storyStatus().ticket;
            ok(first!=null&&first.mission==0&&first.stage==1&&
                first.participants==3&&first.sceneEpoch==1);
            until(()->{
                RoomStoryWire.Ticket t=guest.storyStatus().ticket;
                return t!=null&&t.same(first);
            });
            ok(!host.storyStatus().committed&&!guest.storyStatus().committed);
            int[] guestPlan=guest.guestPreparedStoryFromOriginalFrontend();
            ok(guestPlan.length==8&&guestPlan[0]==1&&guestPlan[1]==0&&
               guestPlan[2]==1&&guestPlan[6]==2&&guestPlan[7]==3);
            ok(host.bindStoryGameThread()&&guest.bindStoryGameThread());
            // No game source means NO READY, NO COMMIT, NO remote NPC rendering.
            ok(host.attestOriginalRunningSceneFromGameThread(10,0,1,0,0,0,0,0)==0);
            ok(guest.attestOriginalRunningSceneFromGameThread(
                20,0,1,first.sceneEpoch,(int)first.rosterRevision,
                (int)(first.token>>>32),(int)first.token,2)==0);
            // If owner actually starts another mission, pending first-stage
            // request must ABORT, not silently label wrong game as ready.
            ok(host.observeOriginalStageIdentityFromGameThread(10,1,0));
            until(()->host.storyStatus().ticket==null&&
                guest.storyStatus().ticket==null);
            ok(host.requestTwoPhoneFirstStageFromLobby()==false);
            // Restart the room so original stage proof begins fresh.
            host.stop();guest.stop();
        }
        try(RoomSession host=RoomSession.create();
            RoomSession guest=RoomSession.create()){
            host.startHost(identity,InetAddress.getByName("127.0.0.1"),0,
                "Host","FIRST STAGE");
            until(()->host.snapshot().phase==RoomSession.Phase.HOSTING);
            RoomSession.Status hs=host.snapshot();
            guest.join("127.0.0.1:"+hs.port,hs.fingerprint,"Second");
            until(()->guest.snapshot().phase==RoomSession.Phase.JOINED&&
                host.snapshot().roster.players.size()==2);
            ok(host.requestTwoPhoneFirstStageFromLobby());
            RoomStoryWire.Ticket t=host.storyStatus().ticket;
            until(()->guest.storyStatus().ticket!=null);
            ok(host.bindStoryGameThread()&&guest.bindStoryGameThread());
            // Simulated genuine native original scene callbacks. Real engine
            // does not auto-launch from the Android UI yet.
            ok(host.observeOriginalStageIdentityFromGameThread(77,0,1));
            // First original loading episode now MUST preserve initial TLS
            // ticket; previous stage=0 incorrectly ABORTED and re-PREPARED.
            ok(host.storyStatus().ticket!=null&&host.storyStatus().ticket.same(t));
            ok(guest.storyStatus().ticket!=null&&guest.storyStatus().ticket.same(t));
            ok(host.attestOriginalRunningSceneFromGameThread(77,0,1,0,0,0,0,0)==1);
            ok(!host.storyStatus().committed);
            ok(guest.observeOriginalStageIdentityFromGameThread(91,0,1));
            ok(guest.storyStatus().ticket!=null&&guest.storyStatus().ticket.same(t));
            ok(guest.attestOriginalRunningSceneFromGameThread(
                91,0,1,t.sceneEpoch,(int)t.rosterRevision,
                (int)(t.token>>>32),(int)t.token,2)==2);
            until(()->host.storyStatus().committed&&guest.storyStatus().committed);
            ok(host.observeOriginalEyeFromGameThread(
                1300,1400,1500,9000,14000,11000));
            until(()->guest.readHostWorldFromGameThread()!=null);
            int[] hostOther=host.otherHumanBodySourceFromGameThread();
            int[] guestOther=guest.otherHumanBodySourceFromGameThread();
            ok(hostOther.length==10&&hostOther[0]==t.sceneEpoch&&
               hostOther[3]==1&&hostOther[4]==2&&
               hostOther[2]>0&&hostOther[5]>9000);
            ok(guestOther.length==10&&guestOther[0]==t.sceneEpoch&&
               guestOther[3]==2&&guestOther[4]==1&&
               guestOther[5]==9000&&guestOther[6]==14000&&
               guestOther[7]==11000);
            RoomWorldSnapshotWire.Snapshot values=guest.readHostWorldFromGameThread();
            ok(values.avatars.size()==2&&values.avatars.get(0).playerId==1&&
               values.avatars.get(1).playerId==2);
            ok(guest.offerHumanInputFromGameThread(
                1,1000,0,0,false,false));
            until(()->!host.drainHumanInputsFromGameThread().isEmpty());
            ok(host.cancelStoryFromGameThread());
            until(()->guest.storyStatus().ticket==null);
            ok(guest.readHostWorldFromGameThread()==null);
            ok(host.otherHumanBodySourceFromGameThread()[0]==0);
            ok(guest.otherHumanBodySourceFromGameThread()[0]==0);
        }
        System.out.println("TWO_PHONE_FIRST_STAGE_START_REAL_TLS_PASS: UI-start PREPARE, two device original-source synthetic READY/COMMIT, host two poses, guest input, ABORT; NO AUTO LAUNCH, NO VISIBLE BOTS");
    }
}
