package com.racas.black.coop;
import java.io.IOException;
import java.util.Arrays;
public final class RoomObjectEventWireTest {
 private interface Op{void run()throws Exception;}
 private static void yes(boolean v){if(!v)throw new AssertionError("Object event wire");}
 private static void bad(Op op)throws Exception{
  try{op.run();throw new AssertionError("Invalid object accepted");}
  catch(IOException expected){}
 }
 public static void main(String[]args)throws Exception{
  byte[] sid=new byte[16];Arrays.fill(sid,(byte)9);
  RoomStoryWire.Ticket t=new RoomStoryWire.Ticket(sid,77,8,0,1,4,15);
  RoomObjectEventWire.Event src=new RoomObjectEventWire.Event(t,1,10,12345L,1,1);
  byte[] f=RoomObjectEventWire.encode(src);
  yes(f.length==58&&f[5]==RoomWire.OBJECT_EVENT&&f[4]==RoomWire.VERSION);
  RoomObjectEventWire.Event event=RoomObjectEventWire.parse(RoomWire.decode(f));
  yes(event.matches(t)&&event.sequence==1&&event.tick==10&&event.objectKey==12345L);
  bad(()->new RoomObjectEventWire.Event(t,0,10,12345L,1,1));
  bad(()->new RoomObjectEventWire.Event(t,1,0,12345L,1,1));
  bad(()->new RoomObjectEventWire.Event(t,1,10,0L,1,1));
  bad(()->new RoomObjectEventWire.Event(t,1,10,-1L,1,1));
  bad(()->new RoomObjectEventWire.Event(t,1,10,1L,1,0));
  bad(()->new RoomObjectEventWire.Event(t,1,10,1L,2,4));
  bad(()->new RoomObjectEventWire.Event(null,1,10,1L,1,1));
  byte[] old=f.clone();old[4]=8;bad(()->RoomWire.decode(old));
  byte[] invalid=f.clone();invalid[8+48]=9;
  bad(()->RoomObjectEventWire.parse(RoomWire.decode(invalid)));
  bad(()->RoomWire.decode(Arrays.copyOf(f,f.length+1)));
  yes(!event.matches(new RoomStoryWire.Ticket(sid,78,8,0,1,4,15)));
  yes(!event.matches(new RoomStoryWire.Ticket(sid,77,8,0,1,5,15)));
  System.out.println("OBJECT_EVENT_WIRE_PASS: strict 50-byte scope, malformed event rejected; NO BLACK gameplay");
 }
}
