package com.racas.black.coop;
import java.net.InetAddress;
import java.nio.file.Paths;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;
/** Four real encrypted sockets, synthetic host-owned environment events. */
public final class RoomObjectEventTlsTest {
 private interface Check{boolean okay()throws Exception;}
 private static void yes(boolean v){if(!v)throw new AssertionError("Object event TLS");}
 private static void until(Check c)throws Exception{
  long cutoff=System.nanoTime()+TimeUnit.SECONDS.toNanos(12);
  while(!c.okay()){
   if(System.nanoTime()>cutoff)throw new AssertionError("Object event timeout");
   Thread.sleep(12);
  }
 }
  /** Full RoomSession -> real TLS -> guest ORIGINAL GAME-THREAD JNI
   * frame; native scene signals are synthetic here, never a BLACK engine.
   * Ensures -1 abort, 0 no-event and exact 10-int host frame. */
  static void sessionBridge(String identityPath)throws Exception {
   SSLContext context=TlsRoomTest.host(Paths.get(identityPath));
   byte[] fingerprint=TlsRoomTest.fingerprint(Paths.get(identityPath));
   RoomSession.IdentitySource identity=()->
       new RoomSession.HostIdentity(context,fingerprint);
   InetAddress loop=InetAddress.getByName("127.0.0.1");
   try(RoomSession host=RoomSession.create();
       RoomSession guest=RoomSession.create()){
    host.startHost(identity,loop,0,"Host","First");
    until(()->host.snapshot().phase==RoomSession.Phase.HOSTING);
    RoomSession.Status endpoint=host.snapshot();
    guest.join("127.0.0.1:"+endpoint.port,endpoint.fingerprint,"Guest");
    until(()->guest.snapshot().phase==RoomSession.Phase.JOINED&&
               guest.snapshot().roster.players.size()==2&&
               host.snapshot().roster.players.size()==2);
    yes(host.bindStoryGameThread()&&guest.bindStoryGameThread());
    yes(guest.pollOriginalHostObjectEventFrameFromGameThread()[0]==-1);
    yes(host.requestTwoPhoneFirstStageFromLobby());
    RoomStoryWire.Ticket ticket=host.storyStatus().ticket;
    until(()->guest.storyStatus().ticket!=null);
    yes(host.observeOriginalStageIdentityFromGameThread(55,0,1));
    yes(guest.observeOriginalStageIdentityFromGameThread(61,0,1));
    yes(host.attestOriginalRunningSceneFromGameThread(55,0,1,0,0,0,0,0)==1);
    yes(guest.attestOriginalRunningSceneFromGameThread(
         61,0,1,ticket.sceneEpoch,(int)ticket.rosterRevision,
         (int)(ticket.token>>>32),(int)ticket.token,2)==2);
    until(()->host.storyStatus().committed&&guest.storyStatus().committed);
    yes(guest.pollOriginalHostObjectEventFrameFromGameThread()[0]==0);
    yes(!host.offerOriginalHostObjectEventFromGameThread(
         56,1,1,77L,RoomObjectEventWire.DESTROYED,1));
    yes(host.offerOriginalHostObjectEventFromGameThread(
         55,1,1,77L,RoomObjectEventWire.DESTROYED,1));
    final int[][] received=new int[1][];
    until(()->{
     int[] frame=guest.pollOriginalHostObjectEventFrameFromGameThread();
     if(frame.length==10)received[0]=frame;
     return received[0]!=null;
    });
    int[] frame=received[0];
    yes(frame[0]==ticket.sceneEpoch&&frame[1]==ticket.rosterRevision&&
        frame[2]==(int)(ticket.token>>>32)&&frame[3]==(int)ticket.token&&
        frame[4]==1&&frame[5]==1&&frame[6]==0&&frame[7]==77&&
        frame[8]==RoomObjectEventWire.DESTROYED&&frame[9]==1);
    yes(guest.pollOriginalHostObjectEventFrameFromGameThread()[0]==0);
    final boolean[] foreign={true};
    Thread worker=new Thread(()->{
     foreign[0]=guest.pollOriginalHostObjectEventFrameFromGameThread()[0]!=-1;
    });
    worker.start();worker.join();yes(!foreign[0]);
    yes(host.cancelStoryFromGameThread());
    until(()->guest.storyStatus().ticket==null);
    yes(guest.pollOriginalHostObjectEventFrameFromGameThread()[0]==-1);
   }
  }
 public static void main(String[]args)throws Exception{
  SSLContext hostTls=TlsRoomTest.host(Paths.get(args[0]));
  SSLContext guestTls=RoomTls.pinnedContext(TlsRoomTest.fingerprint(Paths.get(args[0])));
  InetAddress ip=InetAddress.getByName("127.0.0.1");
  try(RoomTls.Server host=RoomTls.Server.start(hostTls,ip,0,"Host","Objects");
      RoomTls.Client g2=RoomTls.Client.connect(guestTls,ip,host.port(),"G2");
      RoomTls.Client g3=RoomTls.Client.connect(guestTls,ip,host.port(),"G3");
      RoomTls.Client g4=RoomTls.Client.connect(guestTls,ip,host.port(),"G4")){
   until(()->host.snapshot().players.size()==4&&
             g2.snapshot().players.size()==4&&
             g3.snapshot().players.size()==4&&
             g4.snapshot().players.size()==4);
   RoomStoryWire.Ticket t=host.prepareStory(0,1,100);
   RoomObjectEventWire.Event a=new RoomObjectEventWire.Event(t,1,1,101L,1,1);
   yes(!host.offerObjectEvent(a));
   until(()->g2.storyTicket()!=null&&g3.storyTicket()!=null&&g4.storyTicket()!=null);
   yes(!host.hostSceneLoaded(0,1,100));
   yes(g2.acknowledgeSceneLoaded(0,1,100));
   yes(g3.acknowledgeSceneLoaded(0,1,100));
   yes(g4.acknowledgeSceneLoaded(0,1,100));
   until(()->host.storyCommitted()&&g2.storyCommitted()&&
             g3.storyCommitted()&&g4.storyCommitted());
   yes(host.offerObjectEvent(a));
   yes(!host.offerObjectEvent(a));
   final RoomObjectEventWire.Event[] seen=new RoomObjectEventWire.Event[3];
   until(()->{
    if(seen[0]==null)seen[0]=g2.pollHostObjectEvent();
    if(seen[1]==null)seen[1]=g3.pollHostObjectEvent();
    if(seen[2]==null)seen[2]=g4.pollHostObjectEvent();
    return seen[0]!=null&&seen[1]!=null&&seen[2]!=null;
   });
   for(RoomObjectEventWire.Event e:seen)
    yes(e.matches(t)&&e.sequence==1&&e.objectKey==101L&&e.kind==1);
   yes(g2.pollHostObjectEvent()==null);
   RoomObjectEventWire.Event second=new RoomObjectEventWire.Event(t,2,2,102L,2,3);
   yes(host.offerObjectEvent(second));
   final RoomObjectEventWire.Event[] next=new RoomObjectEventWire.Event[3];
   until(()->{
    if(next[0]==null)next[0]=g2.pollHostObjectEvent();
    if(next[1]==null)next[1]=g3.pollHostObjectEvent();
    if(next[2]==null)next[2]=g4.pollHostObjectEvent();
    return next[0]!=null&&next[1]!=null&&next[2]!=null;
   });
   for(RoomObjectEventWire.Event e:next)yes(e.sequence==2&&e.objectKey==102L);
   // An event with a valid next sequence but a time-travelled source
   // tick must be rejected BEFORE any encrypted packet is sent.
   yes(!host.offerObjectEvent(new RoomObjectEventWire.Event(
       t,3,1,103L,RoomObjectEventWire.ACTIVATED,1)));
   yes(host.offerObjectEvent(new RoomObjectEventWire.Event(
       t,3,3,103L,RoomObjectEventWire.ACTIVATED,1)));
   final RoomObjectEventWire.Event[] third=new RoomObjectEventWire.Event[3];
   until(()->{
    if(third[0]==null)third[0]=g2.pollHostObjectEvent();
    if(third[1]==null)third[1]=g3.pollHostObjectEvent();
    if(third[2]==null)third[2]=g4.pollHostObjectEvent();
    return third[0]!=null&&third[1]!=null&&third[2]!=null;
   });
   for(RoomObjectEventWire.Event e:third)
    yes(e.sequence==3&&e.tick==3&&e.objectKey==103L);
   yes(host.cancelStory());
   until(()->g2.storyTicket()==null&&g3.storyTicket()==null&&g4.storyTicket()==null);
   yes(g2.pollHostObjectEvent()==null);
   yes(!host.offerObjectEvent(new RoomObjectEventWire.Event(t,3,3,103L,1,1)));
  }
  sessionBridge(args[0]);
  System.out.println("OBJECT_EVENT_REAL_TLS_PASS: 4 players exact host-only FIFO, replay+scene abort; no engine door/physics changes");
 }
}
