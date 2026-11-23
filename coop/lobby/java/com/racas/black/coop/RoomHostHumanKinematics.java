// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * HOST-ONLY authoritative VALUES for up to three remote human placeholders.
 *
 * Inputs and actual original player feet arrive from authenticated TLS sockets.
 * Guest source coordinates never become host positions directly: the host
 * validates story scope, compact player slot, sequence and movement speed.
 * Host positions begin at the original host's source-verified player feet,
 * then bounded input is reconciled toward the guests' real original feet.
 *
 * Not a game ENGINE actor, physics/collision, real bot renderer, AI, health,
 * shot/hit authority or mission synchronizer. Real actor creation follows.
 */
final class RoomHostHumanKinematics {
    static final int MAX_CM=20000000, MAX_STEP_CM=30;
    static final int WALK_CM_S=300;
    private static final int OFFSET_CM=75;

    private static final class Slot {
        int x,y,z,yaw,flags,sequence;
        long lastInputNs,lastSourceNs;
        int lastSourceSeq;
        boolean sourceActive;
    }
    private final Slot[] slots=new Slot[5];
    private RoomStoryWire.Ticket ticket;
    private Thread owner;
    private boolean seeded;

    // RoomSession is the sole owner. Called under its own lock on room reset.
    void resetForNewRoom(){
        ticket=null;owner=null;seeded=false;
        for(int i=0;i<slots.length;i++)slots[i]=null;
    }
    private boolean sameOwner(){return owner!=null&&Thread.currentThread()==owner;}
    private boolean scope(RoomStoryWire.Ticket proposed){
        return ticket!=null&&proposed!=null&&ticket.same(proposed);
    }
    private static boolean safe(int c){return c>=-MAX_CM&&c<=MAX_CM;}

    /**
     * Only called for the original validated first-person local player
     * inside an already COMMITTED host story; not a scene-loaded attestation.
     */
    boolean observeHostSourceFeet(RoomStoryWire.Ticket source,int x,int y,int z){
        if(source==null||!safe(x)||!safe(y)||!safe(z)||!source.includes(1))
            return false;
        if(owner!=null&&!sameOwner())return false;
        if(!scope(source)){
            resetForNewRoom();
            owner=Thread.currentThread();
            ticket=source;
        }
        if(!seeded){
            for(int id=2;id<=4;id++){
                if(!ticket.includes(id))continue;
                long proposedX=(long)x+(long)(id-1)*OFFSET_CM;
                if(proposedX>MAX_CM||proposedX< -MAX_CM){
                    resetForNewRoom();return false;
                }
                Slot p=new Slot();
                p.x=(int)proposedX;p.y=y;p.z=z;
                slots[id]=p;
            }
            seeded=true;
        }
        return true;
    }
    /**
     * Must only receive RoomTls.Server.TrustedInput.peerId, not an ID in any
     * guest payload. Caller must have verified bound original BLACK thread
     * and host's current committed ticket. No value affects game RAM.
     */
    boolean acceptAuthenticated(RoomStoryWire.Ticket source,int peer,
                                RoomHumanInputWire.Input input,long nowNs){
        if(!sameOwner()||!seeded||!scope(source)||peer<2||peer>4||
           !ticket.includes(peer)||input==null||!input.matches(ticket)||
           nowNs<=0)return false;
        Slot p=slots[peer];
        if(p==null||input.sequence<=p.sequence)return false;
        long dtNs=p.lastInputNs==0?25000000L:nowNs-p.lastInputNs;
        if(dtNs<0)return false;
        // Network stalls cannot create teleports or accelerated catch-up.
        dtNs=Math.min(50000000L,dtNs);
        double step=((double)WALK_CM_S*dtNs)/1000000000.0;
        final double dx=input.moveX/1000.0, dz=input.moveY/1000.0;
        final double norm=Math.max(1.0,Math.sqrt(dx*dx+dz*dz));
        int vx=(int)Math.round(dx/norm*step), vz=(int)Math.round(dz/norm*step);
        if(vx>MAX_STEP_CM||vx< -MAX_STEP_CM||vz>MAX_STEP_CM||vz< -MAX_STEP_CM)
            return false;
        long nx=(long)p.x+vx,nz=(long)p.z+vz;
        if(nx< -MAX_CM||nx>MAX_CM||nz< -MAX_CM||nz>MAX_CM)return false;
        // Once the real guest original feet have arrived, input updates only
        // intent/buttons; do NOT integrate a second fictional trajectory.
        if(!p.sourceActive){p.x=(int)nx;p.z=(int)nz;}
        p.yaw=input.yaw;
        // These buttons are value-only; fire/jump must never directly invoke
        // original weapon physics, NPC AI or quest state.
        p.flags=(input.fire?1:0)|(input.jump?2:0);
        p.sequence=input.sequence;p.lastInputNs=nowNs;
        return true;
    }
    /**
     * Reconcile the original game's guest feet into HOST-owned visual values.
     * Never copy unbounded guest coordinates into the canonical world:
     * speed-limit in 3D; no teleport, engine actor or physics write.
     */
    boolean reconcileOriginalGuestFeet(RoomStoryWire.Ticket current,int slot,
          RoomCameraEyeWire.Eye sample,long nowNs){
        if(!sameOwner()||!seeded||!scope(current)||
           slot<2||slot>4||!ticket.includes(slot)||sample==null||
           !sample.matches(ticket)||nowNs<=0)return false;
        Slot p=slots[slot];
        if(p==null||sample.sequence<=p.lastSourceSeq||
           (p.lastSourceNs!=0&&nowNs<=p.lastSourceNs))return false;
        final long deltaNs=p.lastSourceNs==0?100000000L:
            Math.min(300000000L,nowNs-p.lastSourceNs);
        final double maxStep=12.0+600.0*(double)deltaNs/1000000000.0;
        final long dx=(long)sample.bodyXcm-p.x;
        final long dy=(long)sample.bodyYcm-p.y;
        final long dz=(long)sample.bodyZcm-p.z;
        final double distance=Math.sqrt((double)dx*dx+(double)dy*dy+
                                        (double)dz*dz);
        final double ratio=distance>maxStep?maxStep/distance:1.0;
        final long nx=(long)p.x+Math.round(dx*ratio);
        final long ny=(long)p.y+Math.round(dy*ratio);
        final long nz=(long)p.z+Math.round(dz*ratio);
        if(nx< -MAX_CM||nx>MAX_CM||ny< -MAX_CM||ny>MAX_CM||
           nz< -MAX_CM||nz>MAX_CM)return false;
        p.x=(int)nx;p.y=(int)ny;p.z=(int)nz;
        p.lastSourceSeq=sample.sequence;p.lastSourceNs=nowNs;
        p.sourceActive=true;
        return true;
    }
    List<RoomWorldSnapshotWire.Avatar> remotePoses(RoomStoryWire.Ticket current)
        throws IOException {
        if(!sameOwner()||!seeded||!scope(current))
            return java.util.Collections.emptyList();
        List<RoomWorldSnapshotWire.Avatar> avatars=new ArrayList<>(3);
        for(int id=2;id<=4;id++){
            Slot p=slots[id];
            if(p!=null)avatars.add(new RoomWorldSnapshotWire.Avatar(
                id,p.x,p.y,p.z,p.yaw,p.flags));
        }
        return avatars;
    }
}
