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
   yes(host.cancelStory());
   until(()->g2.storyTicket()==null&&g3.storyTicket()==null&&g4.storyTicket()==null);
   yes(g2.pollHostObjectEvent()==null);
   yes(!host.offerObjectEvent(new RoomObjectEventWire.Event(t,3,3,103L,1,1)));
  }
  System.out.println("OBJECT_EVENT_REAL_TLS_PASS: 4 players exact host-only FIFO, replay+scene abort; no engine door/physics changes");
 }
}
