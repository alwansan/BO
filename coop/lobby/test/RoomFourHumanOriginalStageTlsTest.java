// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;

/**
 * FOUR humans; genuine encrypted TLS sockets; SYNTHETIC original game-thread
 * source evidence injected by the JVM TEST only. Verifies the exact
 * RoomSession facade now used by original Android stage witnesses; no
 * BLACK XBE engine, shared NPC AI, renderer or game memory writes in JVM.
 */
public final class RoomFourHumanOriginalStageTlsTest {
    private RoomFourHumanOriginalStageTlsTest(){}
    private interface Condition{boolean valid()throws Exception;}
    private static void ok(boolean v){if(!v)throw new AssertionError("Four-human original-stage TLS invariant");}
    private static void until(Condition c)throws Exception{
        long cutoff=System.nanoTime()+TimeUnit.SECONDS.toNanos(10);
        while(!c.valid()){
            if(System.nanoTime()>cutoff)throw new AssertionError("Four-human original-stage timeout");
            Thread.sleep(15);
        }
    }
    private static int attest(RoomSession guest,int nativeEpisode,int slot,
                              RoomStoryWire.Ticket t){
        return guest.attestOriginalRunningSceneFromGameThread(nativeEpisode,
            t.mission,t.stage,t.sceneEpoch,(int)t.rosterRevision,
            (int)(t.token>>>32),(int)t.token,slot);
    }
    private static boolean allStory(RoomSession[] guests,RoomStoryWire.Ticket t){
        for(RoomSession g:guests){
            RoomStoryWire.Ticket current=g.storyStatus().ticket;
            if(current==null||!current.same(t))return false;
        }
        return true;
    }
    private static boolean allCommitted(RoomSession host,RoomSession[] guests){
        if(!host.storyStatus().committed)return false;
        for(RoomSession g:guests)if(!g.storyStatus().committed)return false;
        return true;
    }
    public static void main(String[] args)throws Exception{
        SSLContext tls=TlsRoomTest.host(Paths.get(args[0]));
        byte[] fingerprint=TlsRoomTest.fingerprint(Paths.get(args[0]));
        RoomSession.IdentitySource source=()->new RoomSession.HostIdentity(tls,fingerprint);
        InetAddress loop=InetAddress.getByName("127.0.0.1");
        try(RoomSession host=RoomSession.create();
            RoomSession second=RoomSession.create();
            RoomSession third=RoomSession.create();
            RoomSession fourth=RoomSession.create()){
            RoomSession[] guests={second,third,fourth};
            host.startHost(source,loop,0,"Original host","Four humans");
            until(()->host.snapshot().phase==RoomSession.Phase.HOSTING);
            RoomSession.Status endpoint=host.snapshot();
            String address="127.0.0.1:"+endpoint.port;
            second.join(address,endpoint.fingerprint,"Guest two");
            third.join(address,endpoint.fingerprint,"Guest three");
            fourth.join(address,endpoint.fingerprint,"Guest four");
            until(()->{
                if(host.snapshot().roster.players.size()!=4)return false;
                for(RoomSession g:guests)
                    if(g.snapshot().phase!=RoomSession.Phase.JOINED||
                       g.snapshot().roster.players.size()!=4)return false;
                return true;
            });
            ok(host.bindStoryGameThread());
            for(RoomSession g:guests)ok(g.bindStoryGameThread());
            Set<Integer> wire=new HashSet<>();
            Set<Integer> compact=new HashSet<>();
            int[] actualSlots=new int[3];
            for(int i=0;i<3;i++){
                RoomWire.State state=guests[i].snapshot().roster;
                actualSlots[i]=RoomWire.storySlot(state,state.self);
                ok(actualSlots[i]>=2&&actualSlots[i]<=4);
                ok(compact.add(actualSlots[i]));
                ok(wire.add(state.self));
                ok(Arrays.stream(guests[i].guestPreparedStoryFromOriginalFrontend())
                    .allMatch(n->n==0)); // joining a room NEVER launches a scene
            }
            ok(wire.size()==3&&compact.size()==3);
            // Synthetic source-linked native callback only, not actual BLACK.
            ok(host.observeOriginalStageIdentityFromGameThread(90,2,0));
            ok(host.prepareObservedHostStageFromGameThread());
            RoomStoryWire.Ticket ticket=host.storyStatus().ticket;
            ok(ticket!=null&&ticket.participants==15&&ticket.sceneEpoch==1);
            until(()->allStory(guests,ticket));
            for(int i=0;i<3;i++){
                RoomWire.State state=guests[i].snapshot().roster;
                int[] frontend=guests[i].guestPreparedStoryFromOriginalFrontend();
                ok(frontend.length==8&&frontend[0]==ticket.sceneEpoch&&
                   frontend[1]==2&&frontend[2]==0&&frontend[6]==actualSlots[i]&&
                   frontend[7]==15&&frontend[3]==(int)ticket.rosterRevision&&
                   (((long)frontend[4]<<32)|(frontend[5]&0xffffffffL))==ticket.token);
                ok(!guests[i].storyStatus().committed);
            }
            ok(host.attestOriginalRunningSceneFromGameThread(
                90,2,0,0,0,0,0,0)==1);
            ok(!host.storyStatus().committed);
            for(int i=0;i<3;i++){
                int episode=120+i;
                RoomSession g=guests[i];
                ok(g.observeOriginalStageIdentityFromGameThread(episode,2,0));
                ok(attest(g,episode,actualSlots[i],ticket)==2);
                if(i<2)ok(!host.storyStatus().committed);
            }
            until(()->allCommitted(host,guests));
            ok(host.attestOriginalRunningSceneFromGameThread(
                90,2,0,0,0,0,0,0)==0); // duplicate source cannot rearm READY
            // NOW source-based host first-person body feet seeds three
            // independent value-only proxies, not original game actors.
            ok(host.observeOriginalEyeFromGameThread(
                1200,1400,1700,10000,12000,14000));
            until(()->{
                for(RoomSession g:guests){
                    RoomWorldSnapshotWire.Snapshot w=g.readHostWorldFromGameThread();
                    if(w==null||w.tick<1||w.avatars.size()!=4)return false;
                }
                return true;
            });
            int[] hostBodies=host.allOtherHumanBodiesFromGameThread();
            ok(hostBodies.length==23&&hostBodies[0]==ticket.sceneEpoch&&
               hostBodies[3]==1&&hostBodies[4]==3&&
               hostBodies[5]==2&&hostBodies[11]==3&&hostBodies[17]==4);
            for(int i=0;i<3;i++){
                RoomSession guest=guests[i];
                int[] bodies=guest.allOtherHumanBodiesFromGameThread();
                ok(bodies.length==23&&bodies[0]==ticket.sceneEpoch&&
                   bodies[3]==actualSlots[i]&&bodies[4]==3);
            }
            RoomWorldSnapshotWire.Snapshot initial=second.readHostWorldFromGameThread();
            ok(initial.avatars.get(0).xCm==10000&&initial.avatars.get(0).playerId==1);
            for(int i=0;i<4;i++)ok(initial.avatars.get(i).playerId==i+1);
            ok(second.offerHumanInputFromGameThread(1,1000,0,1500,true,false));
            ok(third.offerHumanInputFromGameThread(1,0,1000,-500,false,true));
            ok(fourth.offerHumanInputFromGameThread(1,-1000,0,0,true,true));
            Set<Integer> inputSlots=new HashSet<>();
            until(()->{
                for(RoomTls.Server.TrustedInput sample:
                        host.drainHumanInputsFromGameThread()){
                    if(sample.value.sequence==1&&sample.playerSlot>=2&&
                       sample.playerSlot<=4)inputSlots.add(sample.playerSlot);
                }
                return inputSlots.size()==3;
            });
            ok(inputSlots.containsAll(Arrays.asList(2,3,4)));
            ok(host.observeOriginalEyeFromGameThread(
                1200,1400,1700,10000,12000,14000));
            until(()->{
                for(RoomSession g:guests){
                    RoomWorldSnapshotWire.Snapshot w=g.readHostWorldFromGameThread();
                    if(w==null||w.tick<2||w.avatars.size()!=4)return false;
                }
                return true;
            });
            for(RoomSession g:guests){
                RoomWorldSnapshotWire.Snapshot world=g.readHostWorldFromGameThread();
                RoomWire.State roster=g.snapshot().roster;
                int[] originalGameFrame=RoomGuestHostWorldSource.pack(
                    ticket,world,roster);
                ok(originalGameFrame[0]==1&&originalGameFrame[3]==4&&
                   originalGameFrame[2]==RoomWire.storySlot(roster,roster.self));
                ok(world.avatars.get(actualSlots[0]-1).xCm>
                   initial.avatars.get(actualSlots[0]-1).xCm);
                ok(world.avatars.get(actualSlots[1]-1).zCm>
                   initial.avatars.get(actualSlots[1]-1).zCm);
                ok(world.avatars.get(actualSlots[2]-1).xCm<
                   initial.avatars.get(actualSlots[2]-1).xCm);
            }
            int priorWireId=fourth.snapshot().roster.self;
            fourth.stop(); // network leave must ABORT all committed worlds
            until(()->host.snapshot().roster.players.size()==3&&
                host.storyStatus().ticket==null&&
                second.storyStatus().ticket==null&&
                third.storyStatus().ticket==null);
            ok(!host.storyStatus().committed);
            ok(!second.storyStatus().committed&&!third.storyStatus().committed);
            ok(second.readHostWorldFromGameThread()==null);
            ok(!third.offerHumanInputFromGameThread(
                2,0,0,0,false,false));
            fourth.join(address,endpoint.fingerprint,"Rejoined four");
            until(()->host.snapshot().roster.players.size()==4&&
                fourth.snapshot().phase==RoomSession.Phase.JOINED&&
                fourth.snapshot().roster.players.size()==4);
            ok(fourth.snapshot().roster.self>priorWireId);
            ok(fourth.bindStoryGameThread()); // its new native room generation
            // Same actual original host episode, but new authenticated
            // roster revision => a DIFFERENT TLS story scene epoch.
            ok(!host.observeOriginalStageIdentityFromGameThread(90,2,0));
            ok(host.prepareObservedHostStageFromGameThread());
            RoomStoryWire.Ticket next=host.storyStatus().ticket;
            ok(next!=null&&next.sceneEpoch>ticket.sceneEpoch&&
               !next.same(ticket)&&next.participants==15);
            until(()->allStory(guests,next));
            ok(RoomWire.storySlot(fourth.snapshot().roster,
                fourth.snapshot().roster.self)==4);
            ok(fourth.guestPreparedStoryFromOriginalFrontend()[6]==4);
            ok(host.attestOriginalRunningSceneFromGameThread(
                90,2,0,0,0,0,0,0)==1);
            for(int i=0;i<3;i++){
                // Reconnection alone NEVER attests the new host ticket.
                // Synthetic NEW ORIGINAL loading episodes model three
                // genuinely reloaded local campaign stages. The JNI/native
                // OriginalGuestLaunchMatchGate requires exactly this new
                // source entry, not the old ticket's original stage proof.
                int freshOriginalEpisode=220+i;
                RoomSession g=guests[i];
                int currentCompactSlot=RoomWire.storySlot(
                    g.snapshot().roster,g.snapshot().roster.self);
                ok(currentCompactSlot>=2&&currentCompactSlot<=4);
                ok(g.observeOriginalStageIdentityFromGameThread(
                    freshOriginalEpisode,2,0));
                ok(attest(g,freshOriginalEpisode,currentCompactSlot,ticket)==0);
                ok(attest(g,freshOriginalEpisode,currentCompactSlot,next)==2);
            }
            until(()->allCommitted(host,guests));
            ok(host.cancelStoryFromGameThread());
            until(()->{
                for(RoomSession g:guests)
                    if(g.storyStatus().ticket!=null)return false;
                return true;
            });
            ok(!host.storyStatus().committed);
        }
        System.out.println("FOUR_ORIGINAL_STAGE_REAL_TLS_PASS: 4 humans, original source attestation facade, all READY, host authoritative 4 slots, three encrypted inputs and 3 peers' world, abort, rejoin ID+ and new epoch; SYNTHETIC engine callbacks ONLY");
    }
}
