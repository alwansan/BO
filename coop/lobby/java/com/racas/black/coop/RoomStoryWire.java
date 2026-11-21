// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.security.MessageDigest;
import java.util.Arrays;

/**
 * Value-only story handshake frames carried inside an authenticated room TLS
 * socket. No launcher, rendering hook, actor writes or automatic story start.
 * The authenticated sender's player ID is inferred from its socket, NEVER the
 * untrusted payload. A receiving engine must independently prove stage loaded.
 */
public final class RoomStoryWire {
    private RoomStoryWire(){}
    private static final int BODY_BYTES=36;
    public static final class Ticket {
        private final byte[] session;
        public final long token,rosterRevision;
        public final int mission,stage,sceneEpoch,participants;
        public Ticket(byte[] session,long token,long rosterRevision,int mission,int stage,
                      int sceneEpoch,int participants)throws IOException{
            if(session==null||session.length!=16||token<=0||rosterRevision<=0||
               rosterRevision>0xffffffffL||mission<0||mission>7||stage<0||stage>65535||
               sceneEpoch<=0||participants<3||participants>15||(participants&1)==0)
                throw new IOException("Invalid story ticket");
            this.session=session.clone();
            this.token=token;this.rosterRevision=rosterRevision;
            this.mission=mission;this.stage=stage;this.sceneEpoch=sceneEpoch;
            this.participants=participants;
        }
        public byte[] session(){return session.clone();}
        public boolean same(Ticket other){
            return other!=null&&MessageDigest.isEqual(session,other.session)&&
                token==other.token&&rosterRevision==other.rosterRevision&&
                mission==other.mission&&stage==other.stage&&
                sceneEpoch==other.sceneEpoch&&participants==other.participants;
        }
        public boolean includes(int id){
            return id>=1&&id<=4&&(participants&(1<<(id-1)))!=0;
        }
    }
    public static byte[] encode(int type,Ticket ticket)throws IOException{
        if(type<RoomWire.STORY_PREPARE||type>RoomWire.STORY_ABORT||ticket==null)
            throw new IOException("Invalid story operation");
        ByteArrayOutputStream bytes=new ByteArrayOutputStream(BODY_BYTES);
        DataOutputStream out=new DataOutputStream(bytes);
        out.write(ticket.session);out.writeLong(ticket.token);
        out.writeInt((int)ticket.rosterRevision);out.writeByte(ticket.mission);
        out.writeShort(ticket.stage);out.writeInt(ticket.sceneEpoch);
        out.writeByte(ticket.participants);
        if(bytes.size()!=BODY_BYTES)throw new IOException("Wrong story frame size");
        return RoomWire.frame(type,bytes.toByteArray());
    }
    public static Ticket parse(RoomWire.Packet packet)throws IOException{
        if(packet==null||packet.type<RoomWire.STORY_PREPARE||
           packet.type>RoomWire.STORY_ABORT)
            throw new IOException("Unexpected story operation");
        DataInputStream in=RoomWire.body(packet,packet.type);
        if(in.available()!=BODY_BYTES)throw new IOException("Wrong story body length");
        byte[] session=new byte[16];in.readFully(session);
        long token=in.readLong(),revision=in.readInt()&0xffffffffL;
        int mission=in.readUnsignedByte(),stage=in.readUnsignedShort();
        int epoch=in.readInt(),mask=in.readUnsignedByte();
        if(in.available()!=0)throw new IOException("Trailing story bytes");
        return new Ticket(session,token,revision,mission,stage,epoch,mask);
    }
}
