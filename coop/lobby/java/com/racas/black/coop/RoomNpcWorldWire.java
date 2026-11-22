// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.io.*;
import java.security.MessageDigest;
import java.util.*;
/**
 * Authenticated HOST original BLACK NPC states. Bounded <=512-byte TLS
 * packets, at most two chunks/32 source-owned character slots per tick.
 * Guest receives VALUES only, NEVER actor pointers, client AI or game writes.
 */
public final class RoomNpcWorldWire {
 private RoomNpcWorldWire(){}
 public static final int MAX_NPCS=32, PER_CHUNK=24;
 private static final int HEADER=39,PER_NPC=18,MAX_CM=20000000;
 public static final class Npc {
  public final int slot,xCm,yCm,zCm,healthCenti,flags;
  public Npc(int slot,int x,int y,int z,int health,int flags)throws IOException {
   if(slot<0||slot>=MAX_NPCS||x< -MAX_CM||x>MAX_CM||
      y< -MAX_CM||y>MAX_CM||z< -MAX_CM||z>MAX_CM||
      health<0||health>1000000||(flags&~1)!=0)
    throw new IOException("Invalid host original NPC");
   this.slot=slot;xCm=x;yCm=y;zCm=z;healthCenti=health;this.flags=flags;
  }
 }
 public static final class Chunk {
  private final byte[] session;
  public final long token,revision;
  public final int epoch,tick,part,parts;
  public final List<Npc> npcs;
  private Chunk(byte[] session,long token,long revision,int epoch,
                int tick,int part,int parts,List<Npc> npcs)throws IOException {
   if(session==null||session.length!=16||token<=0||revision<=0||
      revision>0xffffffffL||epoch<=0||tick<=0||parts<1||parts>2||
      part<0||part>=parts||npcs==null||npcs.size()>PER_CHUNK||
      (parts==2&&part==0&&npcs.size()!=PER_CHUNK)||
      (parts==2&&part==1&&(npcs.isEmpty()||npcs.size()>8)))
    throw new IOException("Bad original NPC frame scope");
   int previous=-1;
   for(Npc npc:npcs){
    if(npc==null||npc.slot<=previous)
     throw new IOException("Unsorted or duplicated NPC");
    previous=npc.slot;
   }
   this.session=session.clone();this.token=token;this.revision=revision;
   this.epoch=epoch;this.tick=tick;this.part=part;this.parts=parts;
   this.npcs=Collections.unmodifiableList(new ArrayList<>(npcs));
  }
  public boolean matches(RoomStoryWire.Ticket t){
   return t!=null&&MessageDigest.isEqual(session,t.session())&&
     token==t.token&&revision==t.rosterRevision&&epoch==t.sceneEpoch;
  }
  private boolean same(Chunk x){
   return x!=null&&MessageDigest.isEqual(session,x.session)&&
     token==x.token&&revision==x.revision&&epoch==x.epoch&&
     tick==x.tick&&parts==x.parts;
  }
 }
 public static final class World {
  private final byte[] session;
  public final long token,revision;
  public final int epoch,tick;
  public final List<Npc> npcs;
  private World(Chunk c,List<Npc> npcs){
   session=c.session.clone();token=c.token;revision=c.revision;
   epoch=c.epoch;tick=c.tick;this.npcs=npcs;
  }
  public boolean matches(RoomStoryWire.Ticket t){
   return t!=null&&MessageDigest.isEqual(session,t.session())&&
     token==t.token&&revision==t.rosterRevision&&epoch==t.sceneEpoch;
  }
 }
 public static Chunk chunk(RoomStoryWire.Ticket t,int tick,int part,
                           int parts,List<Npc> npcs)throws IOException {
  if(t==null)throw new IOException("No authenticated story");
  return new Chunk(t.session(),t.token,t.rosterRevision,t.sceneEpoch,
                   tick,part,parts,npcs);
 }
 public static List<Chunk> split(RoomStoryWire.Ticket t,int tick,
                                 List<Npc> npcs)throws IOException{
  if(t==null||npcs==null||npcs.size()>MAX_NPCS)
   throw new IOException("Too many NPC sources");
  int prior=-1;
  for(Npc n:npcs){if(n==null||n.slot<=prior)throw new IOException("Invalid NPC ordering");prior=n.slot;}
  if(npcs.size()<=PER_CHUNK)return Collections.singletonList(chunk(t,tick,0,1,npcs));
  return Arrays.asList(chunk(t,tick,0,2,npcs.subList(0,PER_CHUNK)),
                       chunk(t,tick,1,2,npcs.subList(PER_CHUNK,npcs.size())));
 }
 public static byte[] encode(Chunk c)throws IOException{
  if(c==null)throw new IOException("No host NPC chunk");
  ByteArrayOutputStream b=new ByteArrayOutputStream(HEADER+c.npcs.size()*PER_NPC);
  DataOutputStream out=new DataOutputStream(b);
  out.write(c.session);out.writeLong(c.token);out.writeInt((int)c.revision);
  out.writeInt(c.epoch);out.writeInt(c.tick);
  out.writeByte(c.part);out.writeByte(c.parts);out.writeByte(c.npcs.size());
  for(Npc n:c.npcs){
   out.writeByte(n.slot);out.writeInt(n.xCm);out.writeInt(n.yCm);
   out.writeInt(n.zCm);out.writeInt(n.healthCenti);out.writeByte(n.flags);
  }
  if(b.size()!=HEADER+c.npcs.size()*PER_NPC)throw new IOException("NPC length");
  return RoomWire.frame(RoomWire.NPC_WORLD,b.toByteArray());
 }
 public static Chunk parse(RoomWire.Packet packet)throws IOException{
  DataInputStream in=RoomWire.body(packet,RoomWire.NPC_WORLD);
  int total=in.available();
  if(total<HEADER||total>HEADER+PER_CHUNK*PER_NPC)
   throw new IOException("Host NPC oversized");
  byte[] session=new byte[16];in.readFully(session);
  long token=in.readLong(),revision=in.readInt()&0xffffffffL;
  int epoch=in.readInt(),tick=in.readInt(),part=in.readUnsignedByte(),
      parts=in.readUnsignedByte(),count=in.readUnsignedByte();
  if(total!=HEADER+count*PER_NPC)throw new IOException("NPC length mismatch");
  List<Npc> out=new ArrayList<>(count);
  for(int i=0;i<count;i++)
   out.add(new Npc(in.readUnsignedByte(),in.readInt(),in.readInt(),
                   in.readInt(),in.readInt(),in.readUnsignedByte()));
  if(in.available()!=0)throw new IOException("NPC trailing bytes");
  return new Chunk(session,token,revision,epoch,tick,part,parts,out);
 }
 public static World complete(Chunk first,Chunk second)throws IOException{
  if(first==null||first.part!=0||
     (first.parts==1&&second!=null)||
     (first.parts==2&&(second==null||!first.same(second)||
                       second.part!=1||
                       first.npcs.get(first.npcs.size()-1).slot>=
                         second.npcs.get(0).slot)))
   throw new IOException("Incomplete/mixed host NPC snapshot");
  ArrayList<Npc> complete=new ArrayList<>(first.npcs);
  if(second!=null)complete.addAll(second.npcs);
  return new World(first,Collections.unmodifiableList(complete));
 }
}
