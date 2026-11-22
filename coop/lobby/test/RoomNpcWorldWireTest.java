package com.racas.black.coop;
import java.io.IOException;
import java.util.*;
public final class RoomNpcWorldWireTest {
 private interface Action{void run()throws Exception;}
 private static void ok(boolean x){if(!x)throw new AssertionError("host NPC");}
 private static void reject(Action a)throws Exception{
  boolean failed=false;try{a.run();}catch(IOException e){failed=true;}ok(failed);
 }
 public static void main(String[]args)throws Exception{
  byte[] s=new byte[16];Arrays.fill(s,(byte)7);
  RoomStoryWire.Ticket ticket=new RoomStoryWire.Ticket(s,10L,2L,0,1,9,3);
  List<RoomNpcWorldWire.Npc> list=new ArrayList<>();
  for(int i=0;i<32;i++)list.add(new RoomNpcWorldWire.Npc(i,i*100,500,-300,1000+i,i%2));
  List<RoomNpcWorldWire.Chunk> parts=RoomNpcWorldWire.split(ticket,44,list);
  ok(parts.size()==2);
  RoomNpcWorldWire.Chunk a=RoomNpcWorldWire.parse(RoomWire.decode(RoomNpcWorldWire.encode(parts.get(0))));
  RoomNpcWorldWire.Chunk b=RoomNpcWorldWire.parse(RoomWire.decode(RoomNpcWorldWire.encode(parts.get(1))));
  ok(RoomNpcWorldWire.encode(a).length<=RoomWire.MAX_FRAME&&
     RoomNpcWorldWire.encode(b).length<=RoomWire.MAX_FRAME);
  RoomNpcWorldWire.World world=RoomNpcWorldWire.complete(a,b);
  ok(world.matches(ticket)&&world.npcs.size()==32&&world.tick==44);
  reject(()->RoomNpcWorldWire.complete(b,a));
  reject(()->RoomNpcWorldWire.complete(a,null));
  reject(()->RoomNpcWorldWire.complete(a,
     RoomNpcWorldWire.chunk(ticket,45,1,2,b.npcs)));
  ok(RoomNpcWorldWire.complete(
     RoomNpcWorldWire.split(ticket,45,Collections.emptyList()).get(0),null).npcs.isEmpty());
  ok(RoomNpcWorldWire.split(ticket,46,list.subList(0,25)).size()==2);
  reject(()->RoomNpcWorldWire.split(ticket,44,Arrays.asList(list.get(1),list.get(0))));
  reject(()->new RoomNpcWorldWire.Npc(32,1,1,1,1,0));
  reject(()->new RoomNpcWorldWire.Npc(0,1,1,1,1,2));
  reject(()->new RoomNpcWorldWire.Npc(0,1,1,1,-1,1));
  byte[] wrong=new byte[16];Arrays.fill(wrong,(byte)8);
  ok(!world.matches(new RoomStoryWire.Ticket(wrong,10L,2L,0,1,9,3)));
  System.out.println("ORIGINAL_HOST_NPC_WORLD_TLS_WIRE_PASS: 32 NPCs in <=512B signed fragments; No original game writes");
 }
}
