package com.racas.black.coop;

import java.io.IOException;
import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;

/**
 * Four REAL TLS sockets. Source-camera values are SYNTHETIC FIXTURES in this
 * test, NOT sampled from BLACK's actual running local-character update.
 * No original game loader, bot body rendering or shared AI simulation.
 */
public final class RoomCameraEyeTlsTest {
    static void ok(boolean yes){if(!yes)throw new AssertionError("Camera TLS invariant");}
    interface Condition{boolean ready()throws Exception;}
    static void until(Condition predicate)throws Exception{
        long stop=System.nanoTime()+TimeUnit.SECONDS.toNanos(10);
        while(!predicate.ready()){
            if(System.nanoTime()>stop)throw new AssertionError("Camera TLS timeout");
            Thread.sleep(10);
        }
    }
    static RoomWorldSnapshotWire.Avatar avatar(int id,int x)throws IOException{
        return new RoomWorldSnapshotWire.Avatar(id,x,0,300,0,0);
    }
    static boolean hasStoryTick(RoomTls.Client guest,int tick,RoomStoryWire.Ticket story,
                                int count){
        RoomWorldSnapshotWire.Snapshot snapshot=guest.worldSnapshot();
        return snapshot!=null&&snapshot.tick==tick&&snapshot.matches(story)&&
               snapshot.avatars.size()==count;
    }
    public static void main(String[]args)throws Exception{
        var keys=Paths.get(args[0]);
        SSLContext hostKeys=TlsRoomTest.host(keys);
        SSLContext trust=RoomTls.pinnedContext(TlsRoomTest.fingerprint(keys));
        InetAddress loop=InetAddress.getByName("127.0.0.1");
        try(RoomTls.Server host=RoomTls.Server.start(hostKeys,loop,0,"Host","Campaign");
            RoomTls.Client a=RoomTls.Client.connect(trust,loop,host.port(),"Guest 2");
            RoomTls.Client b=RoomTls.Client.connect(trust,loop,host.port(),"Guest 3");
            RoomTls.Client c=RoomTls.Client.connect(trust,loop,host.port(),"Guest 4")){
            until(()->host.snapshot().players.size()==4&&
                    a.snapshot().players.size()==4&&
                    b.snapshot().players.size()==4&&
                    c.snapshot().players.size()==4);
            int ia=a.snapshot().self,ib=b.snapshot().self,ic=c.snapshot().self;
            ok(ia!=ib&&ia!=ic&&ib!=ic);
            // No original local eye leaks before story commit.
            ok(!a.offerOriginalCameraEye(1,100,0,0,101,0,0));
            ok(host.latestVerifiedCameraEyes().isEmpty());
            var story=host.prepareStory(0,0,7);
            until(()->a.storyTicket()!=null&&b.storyTicket()!=null&&c.storyTicket()!=null);
            ok(!a.offerOriginalCameraEye(1,100,0,0,101,0,0));
            ok(!host.hostSceneLoaded(0,0,7));
            ok(a.acknowledgeSceneLoaded(0,0,7));
            ok(b.acknowledgeSceneLoaded(0,0,7));
            ok(c.acknowledgeSceneLoaded(0,0,7));
            until(()->host.storyCommitted()&&a.storyCommitted()&&
                    b.storyCommitted()&&c.storyCommitted());
            ok(a.offerOriginalCameraEye(1,111,112,113,11,12,13));
            ok(b.offerOriginalCameraEye(1,211,212,213,21,22,23));
            ok(c.offerOriginalCameraEye(1,311,312,313,31,32,33));
            ok(!a.offerOriginalCameraEye(1,900,900,900,90,90,90));
            Map<Integer,RoomCameraEyeWire.Eye> seen=new HashMap<>();
            until(()->{
                for(RoomTls.Server.TrustedEye trusted:host.latestVerifiedCameraEyes())
                    seen.put(trusted.peerId,trusted.camera);
                return seen.size()==3;
            });
            ok(seen.get(ia).xCm==111&&seen.get(ib).yCm==212&&seen.get(ic).zCm==313);
            ok(seen.get(ia).bodyXcm==11&&seen.get(ib).bodyYcm==22&&seen.get(ic).bodyZcm==33);
            for(RoomCameraEyeWire.Eye eye:seen.values())ok(eye.matches(story));
            List<RoomWorldSnapshotWire.Avatar> avatars=new ArrayList<>();
            avatars.add(avatar(1,0));
            int[] ids={ia,ib,ic};Arrays.sort(ids);
            for(int id:ids)avatars.add(avatar(id,seen.get(id).bodyXcm));
            ok(host.offerWorldSnapshot(1,avatars));
            until(()->hasStoryTick(a,1,story,4)&&hasStoryTick(b,1,story,4)&&
                    hasStoryTick(c,1,story,4));
            // The original source sample must not retain stale guest coords.
            Thread.sleep(350);
            ok(host.latestVerifiedCameraEyes().isEmpty());
            ok(host.cancelStory());
            until(()->a.storyTicket()==null&&b.storyTicket()==null&&c.storyTicket()==null);
            ok(!a.offerOriginalCameraEye(2,999,999,999,99,99,99));
            ok(host.latestVerifiedCameraEyes().isEmpty());
            var newStory=host.prepareStory(1,0,8);
            until(()->a.storyTicket()!=null&&b.storyTicket()!=null&&c.storyTicket()!=null);
            ok(!host.hostSceneLoaded(1,0,8));
            ok(a.acknowledgeSceneLoaded(1,0,8));
            ok(b.acknowledgeSceneLoaded(1,0,8));
            ok(c.acknowledgeSceneLoaded(1,0,8));
            until(()->host.storyCommitted()&&a.storyCommitted()&&
                    b.storyCommitted()&&c.storyCommitted());
            ok(!story.same(newStory));
            ok(host.latestVerifiedCameraEyes().isEmpty());
            ok(a.offerOriginalCameraEye(1,-111,0,0,-11,0,0));
            until(()->host.latestVerifiedCameraEyes().size()==1);
            ok(host.latestVerifiedCameraEyes().get(0).camera.matches(newStory));
            ok(!host.latestVerifiedCameraEyes().get(0).camera.matches(story));
        }
        System.out.println("ORIGINAL_EYE_AND_BODY_FEET_TLS_PASS: host+3 guests source-eye wire, authenticated socket IDs, story/roster/epoch, stale expiry, fanout, abort+restart");
        System.out.println("This test's xyz are SYNTHETIC; original BLACK sampled-eye JNI is source-staged, no actual stage/actor gameplay verified");
    }
}
