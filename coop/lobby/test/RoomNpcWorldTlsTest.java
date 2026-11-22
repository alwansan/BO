// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;
/** REAL TLS host-to-guest original-NPC state transport; game source callbacks
 * remain SYNTHETIC here and do NOT imply shared BLACK AI or actor mutations. */
public final class RoomNpcWorldTlsTest {
 private interface Check{boolean yes()throws Exception;}
 private static void ok(boolean v){if(!v)throw new AssertionError("host original NPC TLS");}
 private static void until(Check c)throws Exception {
  long stop=System.nanoTime()+TimeUnit.SECONDS.toNanos(12);
  while(!c.yes()){
   if(System.nanoTime()>stop)throw new AssertionError("original NPC snapshot timeout");
   Thread.sleep(12);
  }
 }
 public static void main(String[]args)throws Exception {
  SSLContext context=TlsRoomTest.host(Paths.get(args[0]));
  byte[] fp=TlsRoomTest.fingerprint(Paths.get(args[0]));
  RoomSession.IdentitySource identity=()->new RoomSession.HostIdentity(context,fp);
  try(RoomSession host=RoomSession.create();RoomSession guest=RoomSession.create()){
   host.startHost(identity,InetAddress.getByName("127.0.0.1"),0,"Host","NPC world");
   until(()->host.snapshot().phase==RoomSession.Phase.HOSTING);
   RoomSession.Status endpoint=host.snapshot();
   guest.join("127.0.0.1:"+endpoint.port,endpoint.fingerprint,"Guest");
   until(()->host.snapshot().roster.players.size()==2&&
         guest.snapshot().phase==RoomSession.Phase.JOINED&&
         guest.snapshot().roster.players.size()==2);
   ok(host.bindStoryGameThread());
   ok(guest.bindStoryGameThread());
   ArrayList<RoomNpcWorldWire.Npc> npcs=new ArrayList<>();
   for(int i=0;i<32;i++)
    npcs.add(new RoomNpcWorldWire.Npc(i,i*90,0,-i*70,8000-i*30,i%2));
   ok(!host.publishOriginalHostNpcWorldFromGameThread(1,npcs));
   ok(guest.readOriginalHostNpcWorldFromGameThread()==null);
   ok(host.requestTwoPhoneFirstStageFromLobby());
   RoomStoryWire.Ticket t=host.storyStatus().ticket;
   until(()->guest.storyStatus().ticket!=null&&
      guest.storyStatus().ticket.same(t));
   ok(host.observeOriginalStageIdentityFromGameThread(55,0,1));
   ok(guest.observeOriginalStageIdentityFromGameThread(61,0,1));
   ok(host.attestOriginalRunningSceneFromGameThread(55,0,1,0,0,0,0,0)==1);
   ok(guest.attestOriginalRunningSceneFromGameThread(
       61,0,1,t.sceneEpoch,(int)t.rosterRevision,
       (int)(t.token>>>32),(int)t.token,2)==2);
   until(()->host.storyStatus().committed&&guest.storyStatus().committed);
   // First actual source sample can only originate on original game
   // thread and must match the host's independent native source gate.
   ok(!host.publishOriginalHostNpcWorldFromGameThread(56,npcs));
   ok(host.publishOriginalHostNpcWorldFromGameThread(55,npcs));
   until(()->{
    RoomNpcWorldWire.World v=guest.readOriginalHostNpcWorldFromGameThread();
    return v!=null&&v.tick>0&&v.npcs.size()==32;
   });
   RoomNpcWorldWire.World first=guest.readOriginalHostNpcWorldFromGameThread();
   ok(first.matches(t)&&first.npcs.get(31).slot==31);
   ok(!host.publishOriginalHostNpcWorldFromGameThread(56,npcs));
   ok(host.publishOriginalHostNpcWorldFromGameThread(55,Collections.emptyList()));
   until(()->{
    RoomNpcWorldWire.World v=guest.readOriginalHostNpcWorldFromGameThread();
    return v!=null&&v.tick>first.tick&&v.npcs.isEmpty();
   });
   ok(host.cancelStoryFromGameThread());
   until(()->guest.storyStatus().ticket==null);
   ok(guest.readOriginalHostNpcWorldFromGameThread()==null);
   ok(!host.publishOriginalHostNpcWorldFromGameThread(55,npcs));
  }
  System.out.println("ORIGINAL_NPC_WORLD_REAL_TLS_PASS: 32 host NPC values, 2 complete chunks, source ticket, empty/reset, no guest engine writes");
 }
}
