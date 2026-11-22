// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;
/** REAL encrypted four-human room; FIRST-stage UI START and ticket propagation.
 * The BLACK original engine is NOT running in this JVM contract test. */
public final class RoomFirstStageFourHumanStartTlsTest {
 private interface Check { boolean ready()throws Exception; }
 private static void ok(boolean b){if(!b)throw new AssertionError("four-human room start");}
 private static void until(Check c)throws Exception {
  long limit=System.nanoTime()+TimeUnit.SECONDS.toNanos(15);
  while(!c.ready()){
   if(System.nanoTime()>limit)throw new AssertionError("START timeout");
   Thread.sleep(15);
  }
 }
 public static void main(String[]args)throws Exception{
  SSLContext tls=TlsRoomTest.host(Paths.get(args[0]));
  byte[] fp=TlsRoomTest.fingerprint(Paths.get(args[0]));
  RoomSession.IdentitySource owner=()->new RoomSession.HostIdentity(tls,fp);
  try(RoomSession host=RoomSession.create();
      RoomSession second=RoomSession.create();
      RoomSession third=RoomSession.create();
      RoomSession fourth=RoomSession.create()){
   host.startHost(owner,InetAddress.getByName("127.0.0.1"),0,"Host","4 ORIGINAL");
   until(()->host.snapshot().phase==RoomSession.Phase.HOSTING);
   ok(!host.requestTwoPhoneFirstStageFromLobby()); // no participants
   RoomSession.Status h=host.snapshot();
   String address="127.0.0.1:"+h.port;
   second.join(address,h.fingerprint,"P2");
   third.join(address,h.fingerprint,"P3");
   fourth.join(address,h.fingerprint,"P4");
   RoomSession[] guests={second,third,fourth};
   until(()->{
    if(host.snapshot().roster.players.size()!=4)return false;
    for(RoomSession g:guests)
     if(g.snapshot().phase!=RoomSession.Phase.JOINED||
        g.snapshot().roster.players.size()!=4)return false;
    return true;
   });
   ok(host.requestTwoPhoneFirstStageFromLobby());
   ok(!host.requestTwoPhoneFirstStageFromLobby()); // prevent replay
   RoomStoryWire.Ticket t=host.storyStatus().ticket;
   ok(t!=null&&t.mission==0&&t.stage==1&&t.participants==15);
   until(()->{
    for(RoomSession g:guests){
     RoomStoryWire.Ticket other=g.storyStatus().ticket;
     if(other==null||!other.same(t))return false;
    }
    return true;
   });
   int[] sender=host.firstStageAutoMenuPlanFromOriginalFrontend();
   ok(sender.length==8&&sender[0]==t.sceneEpoch&&
      sender[1]==0&&sender[2]==1&&sender[6]==1&&sender[7]==15);
   for(RoomSession guest:guests){
    RoomWire.State roster=guest.snapshot().roster;
    int index=RoomWire.storySlot(roster,roster.self);
    int[] intent=guest.firstStageAutoMenuPlanFromOriginalFrontend();
    ok(index>=2&&index<=4&&intent[0]==t.sceneEpoch&&
       intent[1]==0&&intent[2]==1&&intent[6]==index&&intent[7]==15);
    ok(guest.guestPreparedStoryFromOriginalFrontend()[6]==index);
    ok(!guest.storyStatus().committed); // START not READY/COMMIT
   }
   ok(!host.storyStatus().committed);
   ok(host.bindStoryGameThread());
   ok(host.cancelStoryFromGameThread());
   until(()->{
    for(RoomSession g:guests)
     if(g.storyStatus().ticket!=null)return false;
    return true;
   });
   ok(host.firstStageAutoMenuPlanFromOriginalFrontend()[0]==0);
   for(RoomSession g:guests)
    ok(g.firstStageAutoMenuPlanFromOriginalFrontend()[0]==0);
  }
  System.out.println("FOUR_HUMAN_FIRST_STAGE_START_TLS_PASS: 4 signed slot-scoped auto-menu intents, fail-closed ABORT; NO original engine loader/AI");
 }
}
