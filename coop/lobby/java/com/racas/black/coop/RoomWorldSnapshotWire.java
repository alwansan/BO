// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.*;
import java.security.MessageDigest;
import java.util.*;

/**
 * Host -> guests, value-only state extracted from a REAL BLACK host game
 * callback (future integration). No synthetic game world is implemented here.
 * This preserves one host-origin canonical VALUE state on all recipients.
 * Host player position comes from original BLACK actor feet; remote HUMAN
 * preview positions come from HOST kinematic integration of authenticated
 * peer commands, NOT guest-submitted camera/feet coordinates. No BLACK remote
 * renderer, collision/AI/weapon simulation or synchronized mission yet.
 */
public final class RoomWorldSnapshotWire {
    private RoomWorldSnapshotWire(){}
    public static final int MAX_AVATARS=4;
    private static final int FIXED=37, PER_AVATAR=19, MAX_CM=20000000;
    public static final class Avatar {
        public final int playerId,xCm,yCm,zCm,yawCentiDeg,flags;
        public Avatar(int id,int x,int y,int z,int yaw,int flags)throws IOException{
            if(id<1||x< -MAX_CM||x>MAX_CM||y< -MAX_CM||y>MAX_CM||
               z< -MAX_CM||z>MAX_CM||yaw< -18000||yaw>18000||(flags&~3)!=0)
                throw new IOException("Invalid authoritative avatar value");
            playerId=id;xCm=x;yCm=y;zCm=z;yawCentiDeg=yaw;this.flags=flags;
        }
    }
    public static final class Snapshot {
        private final byte[] session;
        public final long token,rosterRevision;
        public final int sceneEpoch,tick;
        public final List<Avatar> avatars;
        private Snapshot(byte[] session,long token,long revision,int epoch,int tick,
                         List<Avatar> avatars)throws IOException {
            if(session==null||session.length!=16||token<=0||revision<=0||
               revision>0xffffffffL||epoch<=0||tick<=0||
               avatars==null||avatars.isEmpty()||avatars.size()>MAX_AVATARS)
                throw new IOException("Invalid authoritative snapshot header");
            int previous=0;
            for(Avatar avatar:avatars){
                if(avatar==null||avatar.playerId<=previous)
                    throw new IOException("Duplicate or unordered avatar ID");
                previous=avatar.playerId;
            }
            if(avatars.get(0).playerId!=1)
                throw new IOException("Missing original host");
            this.session=session.clone();this.token=token;
            rosterRevision=revision;sceneEpoch=epoch;this.tick=tick;
            this.avatars=Collections.unmodifiableList(new ArrayList<>(avatars));
        }
        public boolean matches(RoomStoryWire.Ticket ticket){
            return ticket!=null&&
                MessageDigest.isEqual(session,ticket.session())&&
                token==ticket.token&&rosterRevision==ticket.rosterRevision&&
                sceneEpoch==ticket.sceneEpoch;
        }
    }
    public static Snapshot create(RoomStoryWire.Ticket ticket,int tick,
                                  List<Avatar> avatars)throws IOException{
        if(ticket==null)throw new IOException("Missing host story");
        return new Snapshot(ticket.session(),ticket.token,ticket.rosterRevision,
                            ticket.sceneEpoch,tick,avatars);
    }
    public static byte[] encode(Snapshot snapshot)throws IOException {
        if(snapshot==null)throw new IOException("Missing snapshot");
        ByteArrayOutputStream bytes=new ByteArrayOutputStream(FIXED+snapshot.avatars.size()*PER_AVATAR);
        DataOutputStream out=new DataOutputStream(bytes);
        out.write(snapshot.session);out.writeLong(snapshot.token);
        out.writeInt((int)snapshot.rosterRevision);
        out.writeInt(snapshot.sceneEpoch);out.writeInt(snapshot.tick);
        out.writeByte(snapshot.avatars.size());
        for(Avatar avatar:snapshot.avatars){
            out.writeInt(avatar.playerId);
            out.writeInt(avatar.xCm);out.writeInt(avatar.yCm);out.writeInt(avatar.zCm);
            out.writeShort(avatar.yawCentiDeg);out.writeByte(avatar.flags);
        }
        if(bytes.size()!=FIXED+PER_AVATAR*snapshot.avatars.size())
            throw new IOException("Snapshot size mismatch");
        return RoomWire.frame(RoomWire.WORLD_SNAPSHOT,bytes.toByteArray());
    }
    public static Snapshot parse(RoomWire.Packet packet)throws IOException{
        DataInputStream in=RoomWire.body(packet,RoomWire.WORLD_SNAPSHOT);
        int total=in.available();
        if(total<FIXED+PER_AVATAR||total>FIXED+PER_AVATAR*MAX_AVATARS)
            throw new IOException("Snapshot size exceeds bounded frame");
        byte[] session=new byte[16];in.readFully(session);
        long token=in.readLong(),revision=in.readInt()&0xffffffffL;
        int epoch=in.readInt(),tick=in.readInt(),n=in.readUnsignedByte();
        if(n<1||n>MAX_AVATARS||total!=FIXED+n*PER_AVATAR)
            throw new IOException("Wrong snapshot cardinality");
        ArrayList<Avatar> avatars=new ArrayList<>(n);
        for(int i=0;i<n;i++){
            avatars.add(new Avatar(in.readInt(),in.readInt(),in.readInt(),
                                   in.readInt(),in.readShort(),in.readUnsignedByte()));
        }
        if(in.available()!=0)throw new IOException("Trailing snapshot bytes");
        return new Snapshot(session,token,revision,epoch,tick,avatars);
    }
}
