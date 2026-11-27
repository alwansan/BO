// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.io.*;
import java.security.MessageDigest;

/** Bounded host-sourced event for a stable original BLACK world object.
 * No source object hook, native guest mutation, AI or shared world is
 * implemented by this transport. Object IDs must NEVER be game pointers.
 */
public final class RoomObjectEventWire {
 private RoomObjectEventWire(){}
 public static final int DESTROYED=1,ACTIVATED=2,OBJECTIVE=3,PART_DEPLETED=4;
 private static final int SIZE=50,PART_SIZE=SIZE+10;
 public static final class PartDepletion {
  public final int index,count,definition,healthBits;
  private PartDepletion(int index,int count,int definition,int healthBits)throws IOException{
   if(index<0||index>=count||count>255||definition==-1||
      healthBits<=0||healthBits>=0x7f800000)
    throw new IOException("Invalid original prop part scope");
   this.index=index;this.count=count;this.definition=definition;this.healthBits=healthBits;
  }
 }
 // The story ticket supplies level/stage. The key carries the authored
 // unit index, record count and record ordinal.
 public static long propKey(int unitIndex,int records,int ordinal)throws IOException{
  if(unitIndex<0||records<=0||records>65535||ordinal<0||ordinal>=records)
   throw new IOException("Invalid authored prop key");
  return (long)unitIndex<<32|(long)records<<16|ordinal;
 }
 public static final class Event{
  private final byte[] session;
  public final long token,rosterRevision,objectKey;
  public final int sceneEpoch,sequence,tick,kind,state;
  public final PartDepletion part;
  public Event(RoomStoryWire.Ticket t,int seq,int tick,long objectKey,int kind,int state)throws IOException{
   this(t==null?null:t.session(),t==null?0:t.token,t==null?0:t.rosterRevision,
        t==null?0:t.sceneEpoch,seq,tick,objectKey,kind,state,null);
  }
  private Event(byte[] sid,long token,long revision,int epoch,int seq,int tick,
                long key,int kind,int state,PartDepletion part)throws IOException{
   if(sid==null||sid.length!=16||token<=0||revision<=0||revision>0xffffffffL||
      epoch<=0||seq<=0||tick<=0||key<=0||kind<DESTROYED||kind>PART_DEPLETED||
      state<0||state>3||((kind==DESTROYED||kind==PART_DEPLETED)&&state!=1)||
      (kind==PART_DEPLETED)!=(part!=null))
    throw new IOException("Invalid host world object event");
   if(part!=null)propKey((int)(key>>>32),(int)(key>>>16)&65535,(int)key&65535);
   session=sid.clone();this.token=token;rosterRevision=revision;
   sceneEpoch=epoch;sequence=seq;this.tick=tick;objectKey=key;
   this.kind=kind;this.state=state;this.part=part;
  }
  public boolean matches(RoomStoryWire.Ticket t){
   return t!=null&&MessageDigest.isEqual(session,t.session())&&
    token==t.token&&rosterRevision==t.rosterRevision&&sceneEpoch==t.sceneEpoch;
  }
 }
 public static Event depletedPart(RoomStoryWire.Ticket t,int seq,int tick,long key,
      int index,int count,int definition,int healthBits)throws IOException{
  if(t==null)throw new IOException("Missing story ticket");
  return new Event(t.session(),t.token,t.rosterRevision,t.sceneEpoch,seq,tick,
      key,PART_DEPLETED,1,new PartDepletion(index,count,definition,healthBits));
 }
 public static byte[] encode(Event event)throws IOException{
  if(event==null)throw new IOException("No object event");
  int size=event.part==null?SIZE:PART_SIZE;
  ByteArrayOutputStream bytes=new ByteArrayOutputStream(size);
  DataOutputStream out=new DataOutputStream(bytes);
  out.write(event.session);out.writeLong(event.token);
  out.writeInt((int)event.rosterRevision);out.writeInt(event.sceneEpoch);
  out.writeInt(event.sequence);out.writeInt(event.tick);
  out.writeLong(event.objectKey);out.writeByte(event.kind);out.writeByte(event.state);
  if(event.part!=null){
   out.writeByte(event.part.index);out.writeByte(event.part.count);
   out.writeInt(event.part.definition);out.writeInt(event.part.healthBits);
  }
  if(bytes.size()!=size)throw new IOException("Object event size");
  return RoomWire.frame(RoomWire.OBJECT_EVENT,bytes.toByteArray());
 }
 public static Event parse(RoomWire.Packet packet)throws IOException{
  DataInputStream in=RoomWire.body(packet,RoomWire.OBJECT_EVENT);
  int size=in.available();
  if(size!=SIZE&&size!=PART_SIZE)throw new IOException("Invalid object event length");
  byte[] sid=new byte[16];in.readFully(sid);
  long token=in.readLong(),revision=in.readInt()&0xffffffffL;
  int epoch=in.readInt(),seq=in.readInt(),tick=in.readInt();
  long key=in.readLong();int kind=in.readUnsignedByte(),state=in.readUnsignedByte();
  PartDepletion part=size==PART_SIZE?new PartDepletion(
     in.readUnsignedByte(),in.readUnsignedByte(),in.readInt(),in.readInt()):null;
  if(in.available()!=0)throw new IOException("Trailing object event bytes");
  return new Event(sid,token,revision,epoch,seq,tick,key,kind,state,part);
 }
}
