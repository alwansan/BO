// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.*;
import java.security.MessageDigest;

/**
 * Native ORIGINAL BLACK local camera-eye -> authenticated host TLS socket.
 * Coordinates are captured at original character's first-person update
 * (+0x100 eye) AND original actor BODY FEET (+0xA0), separately sampled.
 * Host world snapshots use BODY FEET rather than CAMERA EYE to locate humans.
 * No rendered actor, source-confirmed guest scene, combat or scene READY yet.
 * Peer ID is derived from TLS socket, never from network frame bytes.
 * Must be accepted only for fully COMMITTED story session/epoch.
 */
public final class RoomCameraEyeWire {
    private RoomCameraEyeWire(){}
    private static final int BYTES=60, LIMIT=20000000;
    public static final class Eye {
        private final byte[] session;
        public final long token,rosterRevision;
        public final int sceneEpoch,sequence,xCm,yCm,zCm,bodyXcm,bodyYcm,bodyZcm;
        public Eye(RoomStoryWire.Ticket ticket,int seq,int x,int y,int z,
                   int bx,int by,int bz)throws IOException{
            this(ticket==null?null:ticket.session(),ticket==null?0:ticket.token,
                 ticket==null?0:ticket.rosterRevision,
                 ticket==null?0:ticket.sceneEpoch,seq,x,y,z,bx,by,bz);
        }
        private Eye(byte[] session,long token,long revision,int epoch,
                    int seq,int x,int y,int z,int bx,int by,int bz)throws IOException{
            if(session==null||session.length!=16||token<=0||revision<=0||
               revision>0xffffffffL||epoch<=0||seq<=0||
               x< -LIMIT||x>LIMIT||y< -LIMIT||y>LIMIT||z< -LIMIT||z>LIMIT||
               bx< -LIMIT||bx>LIMIT||by< -LIMIT||by>LIMIT||bz< -LIMIT||bz>LIMIT)
                throw new IOException("Invalid original camera-eye sample");
            this.session=session.clone();this.token=token;rosterRevision=revision;
            sceneEpoch=epoch;sequence=seq;xCm=x;yCm=y;zCm=z;
            bodyXcm=bx;bodyYcm=by;bodyZcm=bz;
        }
        public boolean matches(RoomStoryWire.Ticket ticket){
            return ticket!=null&&MessageDigest.isEqual(session,ticket.session())&&
                token==ticket.token&&rosterRevision==ticket.rosterRevision&&
                sceneEpoch==ticket.sceneEpoch;
        }
    }
    public static byte[] encode(RoomStoryWire.Ticket ticket,int seq,
                                int x,int y,int z,int bx,int by,int bz)throws IOException{
        Eye eye=new Eye(ticket,seq,x,y,z,bx,by,bz);
        ByteArrayOutputStream bytes=new ByteArrayOutputStream(BYTES);
        DataOutputStream out=new DataOutputStream(bytes);
        out.write(eye.session);out.writeLong(eye.token);
        out.writeInt((int)eye.rosterRevision);
        out.writeInt(eye.sceneEpoch);out.writeInt(eye.sequence);
        out.writeInt(eye.xCm);out.writeInt(eye.yCm);out.writeInt(eye.zCm);
        out.writeInt(eye.bodyXcm);out.writeInt(eye.bodyYcm);out.writeInt(eye.bodyZcm);
        if(bytes.size()!=BYTES)throw new IOException("Camera-eye size mismatch");
        return RoomWire.frame(RoomWire.HUMAN_CAMERA_EYE,bytes.toByteArray());
    }
    public static Eye parse(RoomWire.Packet packet)throws IOException {
        DataInputStream in=RoomWire.body(packet,RoomWire.HUMAN_CAMERA_EYE);
        if(in.available()!=BYTES)throw new IOException("Wrong camera-eye body length");
        byte[] session=new byte[16];in.readFully(session);
        long token=in.readLong(),rev=in.readInt()&0xffffffffL;
        int epoch=in.readInt(),seq=in.readInt();
        int x=in.readInt(),y=in.readInt(),z=in.readInt();
        int bx=in.readInt(),by=in.readInt(),bz=in.readInt();
        if(in.available()!=0)throw new IOException("Camera-eye/body-feet trailing bytes");
        return new Eye(session,token,rev,epoch,seq,x,y,z,bx,by,bz);
    }
}
