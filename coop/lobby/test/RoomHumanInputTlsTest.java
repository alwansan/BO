package com.racas.black.coop;
import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;

/**
 * REAL local TLS sockets, two independent guests -> one host-scoped latest
 * human input per guest. Story loader is SYNTHETIC test attestation only.
 * This is NOT BLACK gameplay, actor movement, crosshair or shared world.
 */
public final class RoomHumanInputTlsTest {
    static void ok(boolean yes){if(!yes)throw new AssertionError("Human TLS input test");}
    interface Condition{boolean ready()throws Exception;}
    static void until(Condition p)throws Exception{
        long end=System.nanoTime()+TimeUnit.SECONDS.toNanos(10);
        while(!p.ready()){
            if(System.nanoTime()>=end)throw new AssertionError("Human TLS timeout");
            Thread.sleep(15);
        }
    }
    public static void main(String[]args)throws Exception{
        SSLContext hostContext=TlsRoomTest.host(Paths.get(args[0]));
        SSLContext guestContext=RoomTls.pinnedContext(TlsRoomTest.fingerprint(Paths.get(args[0])));
        InetAddress addr=InetAddress.getByName("127.0.0.1");
        try(RoomTls.Server host=RoomTls.Server.start(hostContext,addr,0,"Host","Same story");
            RoomTls.Client guest2=RoomTls.Client.connect(guestContext,addr,host.port(),"Two");
            RoomTls.Client guest3=RoomTls.Client.connect(guestContext,addr,host.port(),"Three");
            RoomTls.Client guest4=RoomTls.Client.connect(guestContext,addr,host.port(),"Four")){
            until(()->host.snapshot().players.size()==4&&
                    guest2.snapshot().players.size()==4&&
                    guest3.snapshot().players.size()==4&&
                    guest4.snapshot().players.size()==4);
            int id2=guest2.snapshot().self,id3=guest3.snapshot().self,id4=guest4.snapshot().self;
            ok(id2!=id3&&id2!=id4&&id3!=id4&&id2>1&&id3>1&&id4>1);
            ok(!guest2.offerHumanInput(1,100,100,1200,true,false));
            ok(host.drainLatestTrustedInputs().isEmpty());
            RoomStoryWire.Ticket story=host.prepareStory(0,0,13);
            until(()->guest2.storyTicket()!=null&&guest3.storyTicket()!=null&&guest4.storyTicket()!=null);
            ok(!guest2.offerHumanInput(1,100,0,0,false,false));
            ok(!host.hostSceneLoaded(0,0,13));
            ok(guest2.acknowledgeSceneLoaded(0,0,13));
            ok(guest3.acknowledgeSceneLoaded(0,0,13));
            ok(guest4.acknowledgeSceneLoaded(0,0,13));
            until(()->host.storyCommitted()&&guest2.storyCommitted()&&guest3.storyCommitted()&&guest4.storyCommitted());
            ok(story.includes(1)&&story.includes(2)&&story.includes(3)&&story.includes(4));
            ok(guest2.offerHumanInput(1,-500,150,-12000,true,false));
            ok(guest3.offerHumanInput(1,1000,-1000,18000,false,true));
            ok(guest4.offerHumanInput(1,-200,-200,9000,true,true));
            ok(!guest2.offerHumanInput(1,0,0,0,false,false));
            Map<Integer,RoomHumanInputWire.Input> received=new HashMap<>();
            until(()->{
                for(RoomTls.Server.TrustedInput input:host.drainLatestTrustedInputs())
                    received.put(input.peerId,input.value);
                return received.size()==3;
            });
            ok(received.containsKey(id2)&&received.containsKey(id3)&&received.containsKey(id4));
            ok(received.get(id2).matches(story)&&received.get(id3).matches(story)&&received.get(id4).matches(story));
            ok(received.get(id2).moveX==-500&&received.get(id2).fire);
            ok(received.get(id3).moveX==1000&&received.get(id3).jump);
            ok(received.get(id4).moveX==-200&&received.get(id4).fire&&received.get(id4).jump);
            ok(host.drainLatestTrustedInputs().isEmpty());
            // Send a high-rate burst through one-slot queue. Only newest
            // samples need survive; no socket writes from the caller thread.
            for(int i=2;i<=15;i++)
                ok(guest2.offerHumanInput(i,i*10,0,0,false,false));
            final java.util.concurrent.atomic.AtomicInteger latest=
                new java.util.concurrent.atomic.AtomicInteger();
            until(()->{
                for(RoomTls.Server.TrustedInput input:host.drainLatestTrustedInputs())
                    if(input.peerId==id2)latest.set(input.value.sequence);
                return latest.get()==15;
            });
            ok(host.cancelStory());
            until(()->guest2.storyTicket()==null&&guest3.storyTicket()==null&&guest4.storyTicket()==null);
            ok(!guest2.offerHumanInput(16,0,0,0,false,false));
            ok(host.drainLatestTrustedInputs().isEmpty());
            // Restart story with different epoch: duplicate old input can't
            // be accepted by either endpoint, and sequences restart clean.
            RoomStoryWire.Ticket next=host.prepareStory(1,0,14);
            until(()->guest2.storyTicket()!=null&&guest3.storyTicket()!=null&&guest4.storyTicket()!=null);
            ok(!next.same(story));
            ok(!host.hostSceneLoaded(1,0,14));
            ok(guest2.acknowledgeSceneLoaded(1,0,14));
            ok(guest3.acknowledgeSceneLoaded(1,0,14));
            ok(guest4.acknowledgeSceneLoaded(1,0,14));
            until(()->host.storyCommitted()&&guest2.storyCommitted()&&guest3.storyCommitted()&&guest4.storyCommitted());
            ok(guest2.offerHumanInput(1,700,0,0,false,false));
            until(()->{
                for(RoomTls.Server.TrustedInput input:host.drainLatestTrustedInputs())
                    if(input.peerId==id2&&input.value.matches(next))return true;
                return false;
            });
        }
        System.out.println("HUMAN_INPUT_TLS_PASS: 3 real encrypted guests, 3 distinct authenticated identities, committed story/scene binding, conflated movement, abort/restart");
        System.out.println("NO BLACK gameplay scene, actual player input sampling, NPC model, host actor writes, quest or shared-world motion");
    }
}
