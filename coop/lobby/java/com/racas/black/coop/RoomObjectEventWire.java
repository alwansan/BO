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
 public static final int DESTROYED=1,ACTIVATED=2,OBJECTIVE=3;
 private static final int SIZE=50;
 public static final class Event{
  private final byte[] session;
  public final long token,rosterRevision,objectKey;
  public final int sceneEpoch,sequence,tick,kind,state;
  public Event(RoomStoryWire.Ticket t,int seq,int tick,long objectKey,int kind,int state)throws IOException{
   this(t==null?null:t.session(),t==null?0:t.token,t==null?0:t.rosterRevision,
        t==null?0:t.sceneEpoch,seq,tick,objectKey,kind,state);
  }
  private Event(byte[] sid,long token,long revision,int epoch,int seq,int tick,
                long key,int kind,int state)throws IOException{
   if(sid==null||sid.length!=16||token<=0||revision<=0||revision>0xffffffffL||
      epoch<=0||seq<=0||tick<=0||key<=0||kind<DESTROYED||kind>OBJECTIVE||
      state<0||state>3||(kind==DESTROYED&&state!=1))
    throw new IOException("Invalid host world object event");
   session=sid.clone();this.token=token;rosterRevision=revision;
   sceneEpoch=epoch;sequence=seq;this.tick=tick;objectKey=key;
   this.kind=kind;this.state=state;
  }
  public boolean matches(RoomStoryWire.Ticket t){
   return t!=null&&MessageDigest.isEqual(session,t.session())&&
    token==t.token&&rosterRevision==t.rosterRevision&&sceneEpoch==t.sceneEpoch;
  }
 }
 public static byte[] encode(Event event)throws IOException{
  if(event==null)throw new IOException("No object event");
  ByteArrayOutputStream bytes=new ByteArrayOutputStream(SIZE);
  DataOutputStream out=new DataOutputStream(bytes);
  out.write(event.session);out.writeLong(event.token);
  out.writeInt((int)event.rosterRevision);out.writeInt(event.sceneEpoch);
  out.writeInt(event.sequence);out.writeInt(event.tick);
  out.writeLong(event.objectKey);out.writeByte(event.kind);out.writeByte(event.state);
  if(bytes.size()!=SIZE)throw new IOException("Object event size");
  return RoomWire.frame(RoomWire.OBJECT_EVENT,bytes.toByteArray());
 }
 public static Event parse(RoomWire.Packet packet)throws IOException{
  DataInputStream in=RoomWire.body(packet,RoomWire.OBJECT_EVENT);
  if(in.available()!=SIZE)throw new IOException("Invalid object event length");
  byte[] sid=new byte[16];in.readFully(sid);
  long token=in.readLong(),revision=in.readInt()&0xffffffffL;
  int epoch=in.readInt(),seq=in.readInt(),tick=in.readInt();
  long key=in.readLong();int kind=in.readUnsignedByte(),state=in.readUnsignedByte();
  if(in.available()!=0)throw new IOException("Trailing object event bytes");
  return new Event(sid,token,revision,epoch,seq,tick,key,kind,state);
 }
}
