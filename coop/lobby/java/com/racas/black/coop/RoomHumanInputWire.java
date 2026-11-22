// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.*;
import java.security.MessageDigest;

/**
 * Actual TLS gameplay-input transport format. Values only; never game memory,
 * actor pointers, NPC model identity or untrusted peer/player ID in payload.
 * Host derives player ID from its authenticated TLS peer socket and validates
 * the exact COMMITTED story ticket before accepting a command.
 * Does not by itself connect the BLACK game engine or create a remote actor.
 */
public final class RoomHumanInputWire {
    private RoomHumanInputWire(){}
    private static final int BYTES=43;
    public static final class Input {
        private final byte[] session;
        public final long token,rosterRevision;
        public final int sceneEpoch,sequence,moveX,moveY,yaw;
        public final boolean fire,jump;
        public Input(RoomStoryWire.Ticket ticket,int seq,int x,int y,int angle,
                     boolean fire,boolean jump)throws IOException{
            if(ticket==null||seq<=0||x< -1000||x>1000||y< -1000||y>1000||
                angle< -18000||angle>18000)
                throw new IOException("Unusable movement input");
            session=ticket.session();
            token=ticket.token;rosterRevision=ticket.rosterRevision;
            sceneEpoch=ticket.sceneEpoch;sequence=seq;
            moveX=x;moveY=y;yaw=angle;this.fire=fire;this.jump=jump;
        }
        private Input(byte[] session,long token,long revision,int epoch,int seq,
                      int x,int y,int angle,int flags)throws IOException{
            if(session==null||session.length!=16||token<=0||revision<=0||
               revision>0xffffffffL||epoch<=0||seq<=0||
               x< -1000||x>1000||y< -1000||y>1000||
               angle< -18000||angle>18000||(flags&~3)!=0)
                throw new IOException("Invalid human input frame");
            this.session=session.clone();this.token=token;rosterRevision=revision;
            sceneEpoch=epoch;sequence=seq;
            moveX=x;moveY=y;yaw=angle;fire=(flags&1)!=0;jump=(flags&2)!=0;
        }
        public boolean matches(RoomStoryWire.Ticket ticket){
            return ticket!=null&&
                MessageDigest.isEqual(session,ticket.session())&&
                token==ticket.token&&rosterRevision==ticket.rosterRevision&&
                sceneEpoch==ticket.sceneEpoch;
        }
    }
    public static byte[] encode(RoomStoryWire.Ticket ticket,int seq,
                                int x,int y,int yaw,boolean fire,boolean jump)
        throws IOException {
        Input input=new Input(ticket,seq,x,y,yaw,fire,jump);
        ByteArrayOutputStream buffer=new ByteArrayOutputStream(BYTES);
        DataOutputStream out=new DataOutputStream(buffer);
        out.write(input.session);out.writeLong(input.token);
        out.writeInt((int)input.rosterRevision);out.writeInt(input.sceneEpoch);
        out.writeInt(input.sequence);
        out.writeShort(input.moveX);out.writeShort(input.moveY);
        out.writeShort(input.yaw);
        out.writeByte((input.fire?1:0)|(input.jump?2:0));
        if(buffer.size()!=BYTES)throw new IOException("Input length mismatch");
        return RoomWire.frame(RoomWire.HUMAN_INPUT,buffer.toByteArray());
    }
    public static Input parse(RoomWire.Packet packet)throws IOException{
        DataInputStream in=RoomWire.body(packet,RoomWire.HUMAN_INPUT);
        if(in.available()!=BYTES)throw new IOException("Wrong input frame length");
        byte[] session=new byte[16];in.readFully(session);
        long token=in.readLong(),rev=in.readInt()&0xffffffffL;
        int epoch=in.readInt(),seq=in.readInt();
        int x=in.readShort(),y=in.readShort(),angle=in.readShort(),flags=in.readUnsignedByte();
        if(in.available()!=0)throw new IOException("Trailing input bytes");
        return new Input(session,token,rev,epoch,seq,x,y,angle,flags);
    }
}
