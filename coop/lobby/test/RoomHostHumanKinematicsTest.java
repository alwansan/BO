package com.racas.black.coop;

import java.io.IOException;
import java.util.*;
import java.util.concurrent.atomic.AtomicBoolean;

/** Synthetic, value-only host-side stand-ins, NEVER rendered game bots. */
public final class RoomHostHumanKinematicsTest {
    static void ok(boolean b){if(!b)throw new AssertionError("host human preview");}
    static RoomStoryWire.Ticket ticket(int epoch,long token,int mask)throws IOException{
        byte[] session=new byte[16];
        Arrays.fill(session,(byte)8);
        return new RoomStoryWire.Ticket(session,token,13,0,0,epoch,mask);
    }
    static RoomHumanInputWire.Input input(RoomStoryWire.Ticket t,int seq,int x,int y,
                                          int yaw,boolean fire,boolean jump)throws IOException{
        return new RoomHumanInputWire.Input(t,seq,x,y,yaw,fire,jump);
    }
    static RoomWorldSnapshotWire.Avatar find(List<RoomWorldSnapshotWire.Avatar> all,int id){
        for(RoomWorldSnapshotWire.Avatar a:all)if(a.playerId==id)return a;
        throw new AssertionError("Missing human "+id);
    }
    public static void main(String[]args)throws Exception{
        RoomStoryWire.Ticket first=ticket(1,1,15),changed=ticket(2,2,15);
        RoomHostHumanKinematics host=new RoomHostHumanKinematics();
        ok(!host.observeHostSourceFeet(null,100,200,300));
        ok(!host.observeHostSourceFeet(first,20000000,0,0));
        ok(host.observeHostSourceFeet(first,1000,2000,3000));
        List<RoomWorldSnapshotWire.Avatar> all=host.remotePoses(first);
        ok(all.size()==3);
        ok(find(all,2).xCm==1075&&find(all,3).xCm==1150&&find(all,4).xCm==1225);
        ok(find(all,2).yCm==2000&&find(all,2).zCm==3000);
        ok(!host.acceptAuthenticated(changed,2,input(changed,1,1000,0,0,false,false),100000000L));
        ok(!host.acceptAuthenticated(first,1,input(first,1,1000,0,0,false,false),100000000L));
        ok(!host.acceptAuthenticated(first,5,input(first,1,1000,0,0,false,false),100000000L));
        ok(host.acceptAuthenticated(first,2,input(first,1,1000,0,1000,true,false),100000000L));
        int step=find(host.remotePoses(first),2).xCm;
        ok(step>1075&&step<=1083); // 300cm/s * 25ms = max 7.5cm
        ok(find(host.remotePoses(first),2).flags==1);
        ok(!host.acceptAuthenticated(first,2,input(first,1,-1000,0,0,false,true),125000000L));
        ok(find(host.remotePoses(first),2).xCm==step);
        ok(host.acceptAuthenticated(first,2,input(first,2,1000,1000,500,false,true),125000000L));
        RoomWorldSnapshotWire.Avatar two=find(host.remotePoses(first),2);
        ok(two.xCm>step&&two.zCm>3000&&two.flags==2&&two.yawCentiDeg==500);
        int originalX=two.xCm;
        ok(host.acceptAuthenticated(first,2,input(first,3,-1000,0,0,false,false),10000000000L));
        two=find(host.remotePoses(first),2);
        ok(originalX-two.xCm<=15); // 50ms upper bound despite long lag
        // Observing original host actor again must NOT teleport remote people.
        ok(host.observeHostSourceFeet(first,5000,7000,9000));
        ok(find(host.remotePoses(first),2).xCm==two.xCm);
        AtomicBoolean foreignResult=new AtomicBoolean(true);
        Thread worker=new Thread(()->{
            foreignResult.set(host.observeHostSourceFeet(first,0,0,0));
            try{ok(host.remotePoses(first).isEmpty());}
            catch(IOException e){throw new AssertionError(e);}
        });
        worker.start();worker.join();
        ok(!foreignResult.get());
        // A new committed story invalidates ALL prior coordinates and sequence.
        ok(host.observeHostSourceFeet(changed,100,200,300));
        all=host.remotePoses(changed);
        ok(all.size()==3&&find(all,2).xCm==175);
        ok(host.remotePoses(first).isEmpty());
        ok(host.acceptAuthenticated(changed,2,input(changed,1,0,1000,0,false,false),100000000L));
        host.resetForNewRoom();
        ok(host.remotePoses(changed).isEmpty());
        // Roster includes only host+third human: no other slot may appear.
        RoomStoryWire.Ticket onlyThree=ticket(3,3,5);
        ok(host.observeHostSourceFeet(onlyThree,10,20,30));
        ok(host.remotePoses(onlyThree).size()==1);
        ok(find(host.remotePoses(onlyThree),3).xCm==160);
        System.out.println("HOST_TRUSTED_HUMAN_KINEMATIC_PREVIEW_PASS: 3 host-owned value poses, capped movement, scene/revision reset, foreign-thread rejection, no original NPC/physics");
    }
}
