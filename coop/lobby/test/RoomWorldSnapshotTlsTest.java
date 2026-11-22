package com.racas.black.coop;
import java.io.IOException;
import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;

/**
 * Real 4-party loopback TLS transport, synthetic x/y/z inputs supplied as
 * if an independently verified BLACK HOST world had provided them. NO REAL
 * BLACK actor, scene-loaded callback, remote bot renderer, or world engine.
 */
public final class RoomWorldSnapshotTlsTest {
    static void ok(boolean b){if(!b)throw new AssertionError("World TLS invariant");}
    interface Condition{boolean ready()throws Exception;}
    static void until(Condition condition)throws Exception{
        long stop=System.nanoTime()+TimeUnit.SECONDS.toNanos(10);
        while(!condition.ready()){
            if(System.nanoTime()>stop)throw new AssertionError("Host world fanout timeout");
            Thread.sleep(10);
        }
    }
    static RoomWorldSnapshotWire.Avatar A(int id,int pos)throws IOException{
        return new RoomWorldSnapshotWire.Avatar(id,pos,pos*2,pos*3,500,0);
    }
    static boolean has(RoomTls.Client g,int tick,RoomStoryWire.Ticket story){
        RoomWorldSnapshotWire.Snapshot value=g.worldSnapshot();
        return value!=null&&value.tick==tick&&value.matches(story)&&
               value.avatars.size()==4;
    }
    public static void main(String[]args)throws Exception{
        var identity=Paths.get(args[0]);
        SSLContext serverContext=TlsRoomTest.host(identity);
        SSLContext clientContext=RoomTls.pinnedContext(TlsRoomTest.fingerprint(identity));
        InetAddress loop=InetAddress.getByName("127.0.0.1");
        try(RoomTls.Server host=RoomTls.Server.start(serverContext,loop,0,"Host","Campaign");
            RoomTls.Client two=RoomTls.Client.connect(clientContext,loop,host.port(),"Two");
            RoomTls.Client three=RoomTls.Client.connect(clientContext,loop,host.port(),"Three");
            RoomTls.Client four=RoomTls.Client.connect(clientContext,loop,host.port(),"Four")){
            until(()->host.snapshot().players.size()==4&&
                    two.snapshot().players.size()==4&&
                    three.snapshot().players.size()==4&&
                    four.snapshot().players.size()==4);
            int p2=two.snapshot().self,p3=three.snapshot().self,p4=four.snapshot().self;
            int[] ids={p2,p3,p4};Arrays.sort(ids);
            List<RoomWorldSnapshotWire.Avatar> avatars=new ArrayList<>();
            avatars.add(A(1,100));
            for(int id:ids)avatars.add(A(id,id*100));
            ok(!host.offerWorldSnapshot(1,avatars));
            ok(two.worldSnapshot()==null&&three.worldSnapshot()==null);
            RoomStoryWire.Ticket ticket=host.prepareStory(0,0,24);
            until(()->two.storyTicket()!=null&&three.storyTicket()!=null&&four.storyTicket()!=null);
            ok(!host.offerWorldSnapshot(1,avatars));
            ok(!host.hostSceneLoaded(0,0,24));
            ok(two.acknowledgeSceneLoaded(0,0,24));
            ok(three.acknowledgeSceneLoaded(0,0,24));
            ok(four.acknowledgeSceneLoaded(0,0,24));
            until(()->host.storyCommitted()&&two.storyCommitted()&&
                    three.storyCommitted()&&four.storyCommitted());
            ok(host.offerWorldSnapshot(1,avatars));
            until(()->has(two,1,ticket)&&has(three,1,ticket)&&has(four,1,ticket));
            ok(!host.offerWorldSnapshot(1,avatars)); // replay
            ok(two.worldSnapshot().avatars.get(0).playerId==1);
            ok(two.worldSnapshot().avatars.get(3).playerId==ids[2]);
            // Unknown room peer never becomes a displayed original actor.
            try{
                host.offerWorldSnapshot(2,Arrays.asList(A(1,100),A(999,100)));
                throw new AssertionError("Untrusted avatar accepted");
            }catch(IOException expected){}
            ok(host.offerWorldSnapshot(2,avatars));
            until(()->has(two,2,ticket)&&has(three,2,ticket)&&has(four,2,ticket));
            ok(!host.offerWorldSnapshot(1,avatars));
            // Prevent stale state from remaining indefinitely on screen.
            Thread.sleep(390);
            ok(two.worldSnapshot()==null&&three.worldSnapshot()==null&&four.worldSnapshot()==null);
            ok(host.cancelStory());
            until(()->two.storyTicket()==null&&three.storyTicket()==null&&four.storyTicket()==null);
            ok(!host.offerWorldSnapshot(3,avatars));
            ok(two.worldSnapshot()==null);
            RoomStoryWire.Ticket second=host.prepareStory(1,0,25);
            until(()->two.storyTicket()!=null&&three.storyTicket()!=null&&four.storyTicket()!=null);
            ok(!host.hostSceneLoaded(1,0,25));
            ok(two.acknowledgeSceneLoaded(1,0,25));
            ok(three.acknowledgeSceneLoaded(1,0,25));
            ok(four.acknowledgeSceneLoaded(1,0,25));
            until(()->host.storyCommitted()&&two.storyCommitted()&&
                    three.storyCommitted()&&four.storyCommitted());
            ok(!ticket.same(second));
            ok(host.offerWorldSnapshot(1,avatars)); // resets each new genuine epoch
            until(()->has(two,1,second)&&has(three,1,second)&&has(four,1,second));
        }
        System.out.println("WORLD_TLS_FANOUT_PASS: one host -> three encrypted guests, 4-person authoritative state, replay+roster+epoch/abort+freshness guard");
        System.out.println("NO BLACK real mission, NPC body, sampled engine positions, gameplay or quest replication yet");
    }
}
