// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import javax.net.ssl.SSLContext;

/** Four real TLS endpoints, SYNTHETIC original-game callbacks and descriptors.
 * No native BLACK engine, real objective table, or Android game is running. */
public final class RoomObjectiveSessionTlsTest {
    private interface Condition { boolean ready() throws Exception; }
    private static int cases;
    private static void ok(boolean value, String message) {
        if(!value) throw new AssertionError(message);
    }
    private static void until(String name, Condition condition) throws Exception {
        long end=System.nanoTime()+TimeUnit.SECONDS.toNanos(15);
        while(!condition.ready()) {
            if(System.nanoTime()>end) throw new AssertionError(name+" timeout");
            Thread.sleep(12);
        }
    }
    private static void passed(String name) {
        ++cases;System.out.println("OBJECTIVE_SESSION_CASE_PASS "+name);
    }
    // Fixture specification is independent of host records or TLS packets.
    private static byte[][] ids(int count) {
        byte[][] value=new byte[count][8];
        for(int i=0;i<count;i++) {
            value[i][0]=(byte)i;value[i][1]=(byte)(i>>>8);
            value[i][7]=(byte)0x80;
        }
        return value;
    }
    private static byte[] records(int count,int seed) {
        ByteBuffer b=ByteBuffer.allocate(16*count).order(ByteOrder.LITTLE_ENDIAN);
        for(int i=0;i<count;i++) {
            b.putLong(Long.MIN_VALUE|i);
            b.putInt((i+seed*97)&65535);
            b.putInt((i+seed)&3);
        }
        return b.array();
    }
    private static boolean allReceived(RoomSession[] guests,long seq,int count,int seed) {
        for(RoomSession guest:guests) {
            RoomObjectiveChunks.Snapshot value=guest.readHostObjectivesFromGameThread();
            if(value==null||value.sequence!=seq||
               !Arrays.equals(value.records(),records(count,seed)))return false;
        }
        return true;
    }
    public static void main(String[] args) throws Exception {
        SSLContext tls=TlsRoomTest.host(Paths.get(args[0]));
        byte[] pin=TlsRoomTest.fingerprint(Paths.get(args[0]));
        RoomSession.IdentitySource identity=()->new RoomSession.HostIdentity(tls,pin);
        InetAddress loop=InetAddress.getByName("127.0.0.1");
        try(RoomSession host=RoomSession.create();
            RoomSession g2=RoomSession.create();
            RoomSession g3=RoomSession.create();
            RoomSession g4=RoomSession.create()) {
            RoomSession[] guests={g2,g3,g4};
            host.startHost(identity,loop,0,"Host","Objectives");
            until("hosting",()->host.snapshot().phase==RoomSession.Phase.HOSTING);
            String address="127.0.0.1:"+host.snapshot().port;
            String fingerprint=host.snapshot().fingerprint;
            for(int i=0;i<3;i++)guests[i].join(address,fingerprint,"Guest"+(i+2));
            until("roster",()->{
                if(host.snapshot().roster.players.size()!=4)return false;
                for(RoomSession g:guests)if(g.snapshot().phase!=RoomSession.Phase.JOINED||
                    g.snapshot().roster.players.size()!=4)return false;
                return true;
            });
            ok(host.bindStoryGameThread(),"host thread");
            for(RoomSession g:guests)ok(g.bindStoryGameThread(),"guest thread");
            byte[][] expected=ids(4096);
            ok(!g2.bindOriginalObjectiveIdsFromGameThread(61,expected),"no PREPARE/source");
            ok(g2.readHostObjectivesFromGameThread()==null,"uncommitted objective absent");
            passed("no_prepared_source_no_values");

            ok(host.requestTwoPhoneFirstStageFromLobby(),"host PREPARE");
            RoomStoryWire.Ticket ticket=host.storyStatus().ticket;
            until("guest PREPARE",()->{
                for(RoomSession g:guests){
                    RoomStoryWire.Ticket t=g.storyStatus().ticket;
                    if(t==null||!t.same(ticket))return false;
                }
                return true;
            });
            ok(host.observeOriginalStageIdentityFromGameThread(50,0,1),"host source");
            for(int i=0;i<3;i++) {
                RoomSession g=guests[i];int episode=61+i;
                ok(!g.bindOriginalObjectiveIdsFromGameThread(episode,expected),"source required");
                ok(g.observeOriginalStageIdentityFromGameThread(episode,0,1),"local source");
                ok(!g.bindOriginalObjectiveIdsFromGameThread(episode+1,expected),"episode mismatch");
                byte[][] own=ids(4096);
                ok(g.bindOriginalObjectiveIdsFromGameThread(episode,own),"independent bound");
                own[0][7]^=1;
                ok(!g.bindOriginalObjectiveIdsFromGameThread(episode,ids(4096)),"no rebinding");
            }
            AtomicBoolean crossThreadAccepted=new AtomicBoolean();
            Thread foreign=new Thread(()->{
                crossThreadAccepted.set(g2.bindOriginalObjectiveIdsFromGameThread(61,ids(4096))||
                    g2.readHostObjectivesFromGameThread()!=null);
            },"synthetic-non-game-thread");
            foreign.start();foreign.join();
            ok(!crossThreadAccepted.get(),"game-thread fence");
            passed("independent_pre_ready_binding_and_thread_fence");

            ok(host.attestOriginalRunningSceneFromGameThread(50,0,1,0,0,0,0,0)==1,
                "source host READY");
            for(int i=0;i<3;i++) {
                int slot=RoomWire.storySlot(guests[i].snapshot().roster,
                                             guests[i].snapshot().roster.self);
                ok(guests[i].attestOriginalRunningSceneFromGameThread(61+i,0,1,
                    ticket.sceneEpoch,(int)ticket.rosterRevision,
                    (int)(ticket.token>>>32),(int)ticket.token,slot)==2,
                    "source guest READY");
            }
            until("COMMIT",()->{
                if(!host.storyStatus().committed)return false;
                for(RoomSession g:guests)if(!g.storyStatus().committed)return false;
                return true;
            });
            ok(!g3.bindOriginalObjectiveIdsFromGameThread(62,expected),"no late rebind");
            ok(!host.publishOriginalHostObjectiveSnapshotFromGameThread(51,1,records(4096,0)),
               "wrong host source");
            byte[] offered=records(4096,0);
            ok(host.publishOriginalHostObjectiveSnapshotFromGameThread(50,1,offered),
               "verified host synthetic source offer");
            Arrays.fill(offered,(byte)0);
            until("all objective values",()->{
                host.observeOriginalStageIdentityFromGameThread(50,0,1);
                for(int i=0;i<3;i++)guests[i].observeOriginalStageIdentityFromGameThread(61+i,0,1);
                return allReceived(guests,1,4096,0);
            });
            passed("four_session_tls_4096_exact_complete_values");

            RoomObjectiveChunks.Snapshot retained=g2.readHostObjectivesFromGameThread();
            ok(g2.isHostObjectiveSnapshotCurrentFromGameThread(61,retained),"fresh consume");
            ok(!g2.isHostObjectiveSnapshotCurrentFromGameThread(62,retained),"wrong local episode");
            ok(!g2.isHostObjectiveSnapshotCurrentFromGameThread(61,null),"null consume");
            AtomicBoolean foreignConsume=new AtomicBoolean(true);
            Thread consumer=new Thread(()->foreignConsume.set(
                g2.isHostObjectiveSnapshotCurrentFromGameThread(61,retained)));
            consumer.start();consumer.join();
            ok(!foreignConsume.get(),"retained consume thread fence");
            // Keep the local scene witness fresh while the complete snapshot
            // ages. Render callbacks/reads must never renew snapshot freshness.
            long expiry=System.nanoTime()+TimeUnit.MILLISECONDS.toNanos(3200);
            while(System.nanoTime()<expiry){
                for(int i=0;i<3;i++)guests[i].observeOriginalStageIdentityFromGameThread(61+i,0,1);
                g2.readHostObjectivesFromGameThread();
                Thread.sleep(20);
            }
            for(RoomSession g:guests)ok(g.readHostObjectivesFromGameThread()==null,
                "delayed complete snapshot expires despite fresh scene");
            ok(!g2.isHostObjectiveSnapshotCurrentFromGameThread(61,retained),"delayed retained consume");
            host.observeOriginalStageIdentityFromGameThread(50,0,1);
            ok(host.publishOriginalHostObjectiveSnapshotFromGameThread(50,2,records(4096,1)),"fresh recovery");
            until("fresh recovery received",()->{
                for(int i=0;i<3;i++)guests[i].observeOriginalStageIdentityFromGameThread(61+i,0,1);
                return allReceived(guests,2,4096,1);
            });
            ok(!g2.isHostObjectiveSnapshotCurrentFromGameThread(61,retained),"superseded consume");
            RoomObjectiveChunks.Snapshot beforeScene=g2.readHostObjectivesFromGameThread();
            RoomObjectiveChunks.Snapshot beforeNewer=g3.readHostObjectivesFromGameThread();
            ok(g3.isHostObjectiveSnapshotCurrentFromGameThread(62,beforeNewer),"new sample consumable");
            passed("consume_ttl_episode_thread_and_fresh_recovery");

            ok(g2.observeOriginalStageIdentityFromGameThread(100,0,1),
               "new local episode even with same mission and stage");
            ok(g2.readHostObjectivesFromGameThread()==null,"revoked read");
            ok(!g2.isHostObjectiveSnapshotCurrentFromGameThread(61,beforeScene),"retained scene revoke");
            ok(!g2.isHostObjectiveSnapshotCurrentFromGameThread(100,beforeScene),"new episode cannot revive retained");
            ok(!g2.bindOriginalObjectiveIdsFromGameThread(100,ids(4096)),
               "no same-ticket identity reset after source change");
            ok(g2.storyStatus().committed,"room connection stays committed");
            ok(g3.readHostObjectivesFromGameThread()!=null&&
               g4.readHostObjectivesFromGameThread()!=null,
               "other guest values remain independent");
            passed("new_local_episode_revokes_one_guest_only");

            host.observeOriginalStageIdentityFromGameThread(50,0,1);
            ok(host.publishOriginalHostObjectiveSnapshotFromGameThread(50,3,records(4096,1)),
               "next host sample");
            until("unaffected guests progress",()->{
                host.observeOriginalStageIdentityFromGameThread(50,0,1);
                g3.observeOriginalStageIdentityFromGameThread(62,0,1);
                g4.observeOriginalStageIdentityFromGameThread(63,0,1);
                RoomObjectiveChunks.Snapshot a=g3.readHostObjectivesFromGameThread();
                RoomObjectiveChunks.Snapshot b=g4.readHostObjectivesFromGameThread();
                return a!=null&&b!=null&&a.sequence==3&&b.sequence==3&&
                    Arrays.equals(a.records(),records(4096,1))&&
                    Arrays.equals(b.records(),records(4096,1));
            });
            ok(g2.readHostObjectivesFromGameThread()==null,
               "late old chunks do not restore revoked values");
            ok(g2.snapshot().phase==RoomSession.Phase.JOINED,
               "revoked objective does not disconnect lobby");
            passed("revoked_inflight_parts_ignored_room_stays_live");

            ok(!g3.isHostObjectiveSnapshotCurrentFromGameThread(62,beforeNewer),"new completion supersedes still fresh retained sample");
            RoomObjectiveChunks.Snapshot beforeAbort=g3.readHostObjectivesFromGameThread();
            ok(g3.isHostObjectiveSnapshotCurrentFromGameThread(62,beforeAbort),"current before abort");
            RoomObjectiveChunks.Snapshot beforeMission=g4.readHostObjectivesFromGameThread();
            ok(g4.isHostObjectiveSnapshotCurrentFromGameThread(63,beforeMission),"current before mission change");
            ok(g4.observeOriginalStageIdentityFromGameThread(101,1,2),"different local mission");
            ok(!g4.isHostObjectiveSnapshotCurrentFromGameThread(63,beforeMission),"old mission consume");
            ok(!g4.isHostObjectiveSnapshotCurrentFromGameThread(101,beforeMission),"new mission cannot reuse old values");
            passed("different_local_mission_revokes_retained_completion");
            ok(host.cancelStoryFromGameThread(),"abort current ticket");
            until("all aborted",()->{
                for(RoomSession g:guests)if(g.storyStatus().ticket!=null)return false;
                return true;
            });
            for(RoomSession g:guests)ok(g.readHostObjectivesFromGameThread()==null,
                                         "abort invalidates reads");
            ok(!g3.isHostObjectiveSnapshotCurrentFromGameThread(62,beforeAbort),"retained abort revoke");
            ok(!host.publishOriginalHostObjectiveSnapshotFromGameThread(50,4,records(4096,2)),
               "no post abort publication");
            passed("abort_revokes_all_objective_reads");

            RoomStoryWire.Ticket next=host.prepareStoryFromGameThread(0,1,
                                                  ticket.sceneEpoch+1);
            ok(!ticket.same(next),"new ticket differs");
            ok(!g3.isHostObjectiveSnapshotCurrentFromGameThread(62,beforeAbort),"new ticket cannot revive retained");
            until("new prepared",()->{
                for(RoomSession g:guests){
                    RoomStoryWire.Ticket t=g.storyStatus().ticket;
                    if(t==null||!t.same(next))return false;
                }
                return true;
            });
            for(int i=0;i<3;i++){
                int episode=201+i;
                RoomSession g=guests[i];
                ok(g.observeOriginalStageIdentityFromGameThread(episode,0,1),
                   "fresh original game episode on new ticket");
                ok(g.bindOriginalObjectiveIdsFromGameThread(episode,ids(1)),
                   "new ticket independent binding");
            }
            ok(!host.reportSceneLoadedFromGameThread(0,1,next.sceneEpoch),"host waits again");
            for(RoomSession g:guests)
                ok(g.reportSceneLoadedFromGameThread(0,1,next.sceneEpoch),
                   "new synthetic guest READY");
            until("new commit",()->{
                if(!host.storyStatus().committed)return false;
                for(RoomSession g:guests)if(!g.storyStatus().committed)return false;
                return true;
            });
            // Native ready witness for this fresh ticket must be independent:
            // a first COMMIT alone never authorizes objective game-thread reads.
            for(RoomSession g:guests)
                ok(g.readHostObjectivesFromGameThread()==null,
                    "new commit not an original scene attestation");
            passed("new_ticket_rebind_requires_fresh_native_attestation");
        }
        ok(cases==9,"expected scenario cardinality");
        System.out.println("OBJECTIVE_SESSION_REAL_TLS_PASS scenarios=9 guests=3 records=4096 SYNTHETIC_STAGE_AND_IDS_NO_ANDROID_ENGINE");
    }
}
