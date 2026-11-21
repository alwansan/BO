package com.racas.black.coop;
import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;

/** Real encrypted loopback SOCKETS; story frames only, no game scene or actor. */
public final class RoomStoryTlsTest {
    static void ok(boolean v){if(!v)throw new AssertionError("TLS story preflight");}
    interface Condition {boolean ready()throws Exception;}
    static void until(Condition predicate)throws Exception{
        long deadline=System.nanoTime()+TimeUnit.SECONDS.toNanos(8);
        while(!predicate.ready()){
            if(System.nanoTime()>deadline)throw new AssertionError("Story handshake timeout");
            Thread.sleep(15);
        }
    }
    public static void main(String[]args)throws Exception{
        var identity=Paths.get(args[0]);
        SSLContext hostContext=TlsRoomTest.host(identity);
        SSLContext guestContext=RoomTls.pinnedContext(TlsRoomTest.fingerprint(identity));
        InetAddress loop=InetAddress.getByName("127.0.0.1");
        try(RoomTls.Server host=RoomTls.Server.start(hostContext,loop,0,"Host","Campaign");
            RoomTls.Client guest=RoomTls.Client.connect(guestContext,loop,host.port(),"Remote human")){
            until(()->guest.snapshot()!=null&&guest.snapshot().players.size()==2);
            final long gameThread=Thread.currentThread().getId();
            final java.util.concurrent.atomic.AtomicLong writerThread=
                new java.util.concurrent.atomic.AtomicLong();
            final java.util.concurrent.atomic.AtomicInteger queued=
                new java.util.concurrent.atomic.AtomicInteger();
            final java.util.concurrent.atomic.AtomicInteger sent=
                new java.util.concurrent.atomic.AtomicInteger();
            guest.setSafeEventSink(code->{
                if("STORY_GUEST_READY_QUEUED".equals(code))queued.incrementAndGet();
                if("STORY_GUEST_READY_SENT".equals(code)){
                    writerThread.set(Thread.currentThread().getId());
                    sent.incrementAndGet();
                }
            });
            RoomStoryWire.Ticket ticket=host.prepareStory(0,0,1);
            ok(ticket.mission==0&&ticket.stage==0&&ticket.sceneEpoch==1&&ticket.participants==3);
            ok(!host.storyCommitted()&&!guest.storyCommitted());
            until(()->guest.storyTicket()!=null);
            ok(ticket.same(guest.storyTicket()));
            ok(!host.hostSceneLoaded(0,1,1)); // wrong stage
            ok(!guest.acknowledgeSceneLoaded(7,0,1));
            ok(!guest.acknowledgeSceneLoaded(0,0,2));
            ok(!host.hostSceneLoaded(0,0,1)); // locally loaded but guest not yet READY
            ok(!host.storyCommitted());
            ok(guest.acknowledgeSceneLoaded(0,0,1)); // only enqueues
            ok(!guest.acknowledgeSceneLoaded(0,0,1)); // duplicate
            until(()->host.storyCommitted()&&guest.storyCommitted());
            until(()->sent.get()==1);
            ok(queued.get()==1&&writerThread.get()!=0&&writerThread.get()!=gameThread);
            ok(ticket.same(guest.storyTicket()));
            // Story host barrier cannot silently start a second mission.
            try{host.prepareStory(1,0,2);throw new AssertionError("Duplicate story accepted");}
            catch(java.io.IOException expected){}
            ok(host.cancelStory());
            until(()->guest.storyTicket()==null);
            var second=host.prepareStory(1,0,2);
            ok(second.token!=ticket.token);
            until(()->guest.storyTicket()!=null&&second.same(guest.storyTicket()));
            ok(!host.hostSceneLoaded(1,0,2)); // waits for guest's actual second stage
            ok(guest.acknowledgeSceneLoaded(1,0,2));
            until(()->host.storyCommitted()&&guest.storyCommitted());
            until(()->sent.get()==2);
            ok(queued.get()==2&&writerThread.get()!=gameThread);
            ok(second.same(guest.storyTicket()));
        }
        // A peer exit during PREPARE must revoke the barrier and refuse COMMIT.
        try(RoomTls.Server host=RoomTls.Server.start(hostContext,loop,0,"Host","Campaign")){
            try(RoomTls.Client guest=RoomTls.Client.connect(guestContext,loop,host.port(),"Remote")){
                until(()->guest.snapshot()!=null&&guest.snapshot().players.size()==2);
                host.prepareStory(0,0,11);
                until(()->guest.storyTicket()!=null);
            } // guest exits before any READY; no manual close() on try resources
            until(()->host.snapshot().players.size()==1);
            ok(host.storyTicket()==null&&!host.storyCommitted());
        }
        // Compacted participant slots must NOT rely on permanently increasing
        // room player IDs; old ID is never reused across disconnects.
        try(RoomTls.Server host=RoomTls.Server.start(hostContext,loop,0,"Host","Campaign")){
            int old;
            try(RoomTls.Client first=RoomTls.Client.connect(guestContext,loop,host.port(),"First")){
                old=first.snapshot().self;
            }
            until(()->host.snapshot().players.size()==1);
            try(RoomTls.Client later=RoomTls.Client.connect(guestContext,loop,host.port(),"Later")){
                ok(later.snapshot().self>old);
                var ticket=host.prepareStory(7,0,33);
                until(()->later.storyTicket()!=null);
                ok(ticket.participants==3&&ticket.same(later.storyTicket()));
                ok(!host.hostSceneLoaded(7,0,33));
                ok(later.acknowledgeSceneLoaded(7,0,33));
                until(()->host.storyCommitted()&&later.storyCommitted());
            }
        }
        // Owner CL reported ERROR_BINDEXCEPTION on rapid Host -> Leave -> Host.
        // An active second listener must still be rejected on the same port;
        // rebind AFTER shutdown and completed TLS clients must be allowed.
        int reusablePort;
        try(RoomTls.Server first=RoomTls.Server.start(hostContext,loop,0,"Host","Restart")){
            reusablePort=first.port();
            try(RoomTls.Server illegal=RoomTls.Server.start(hostContext,loop,reusablePort,"Other","Other")){
                throw new AssertionError("Concurrent host unexpectedly bound TCP port "+illegal.port());
            }catch(java.net.BindException expected){}
            try(RoomTls.Client connected=RoomTls.Client.connect(guestContext,loop,reusablePort,"Before leave")){
                ok(connected.snapshot().players.size()==2);
            }
        }
        for(int attempt=0;attempt<4;attempt++){
            try(RoomTls.Server restarted=RoomTls.Server.start(hostContext,loop,reusablePort,"Host","Restart")){
                ok(restarted.port()==reusablePort);
                try(RoomTls.Client joined=RoomTls.Client.connect(guestContext,loop,reusablePort,"Again")){
                    ok(joined.snapshot().players.size()==2);
                }
            }
        }
        System.out.println("ROOM_REHOST_PASS: concurrent listener refused; 4 rapid same-port host restarts with real TLS clients");
        System.out.println("STORY_READY_ASYNC_PASS: READY queued on game callback thread; TLS socket write on separate bounded network worker; no duplicate");
        System.out.println("STORY_TLS_PASS: real authenticated host PREPARE, guest scene READY, host COMMIT, abort on leave, compact slots after rejoin");
        System.out.println("NO BLACK engine scene loading, gameplay actor or phone co-op executed");
    }
}
