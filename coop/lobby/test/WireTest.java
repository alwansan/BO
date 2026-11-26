package com.racas.black.coop;
import java.io.*;
import java.util.*;
public final class WireTest {
 static void check(boolean value){if(!value)throw new AssertionError();}
 interface Attempt{void run()throws Exception;}
 static void rejects(Attempt f)throws Exception{try{f.run();}catch(IOException|IllegalArgumentException expected){return;}throw new AssertionError("accepted bad input");}
 public static void main(String[]args)throws Exception{
  byte[] nonce=new byte[16];Arrays.fill(nonce,(byte)7);
  var join=new RoomWire.Join("علي",nonce);
  byte[] frame=RoomWire.join(join);
  check(RoomWire.parseJoin(RoomWire.decode(frame)).name.equals("علي"));
  byte[] wrong=frame.clone();wrong[4]=99;rejects(()->RoomWire.decode(wrong));
  // Version 9 peers cannot safely consume the new objective chunk stream.
  check(RoomWire.VERSION==10&&RoomWire.PROFILE==10);
  byte[] oldVersion=frame.clone();oldVersion[4]=9;
  rejects(()->RoomWire.decode(oldVersion));
  byte[] oldProfile=frame.clone();oldProfile[8+20]=0;oldProfile[8+20+1]=9;
  rejects(()->RoomWire.parseJoin(RoomWire.decode(oldProfile)));
  // Framing bounds only: the objective assembler separately validates bodies.
  byte[] objectiveFrame=RoomWire.frame(RoomWire.OBJECTIVE_CHUNK,new byte[504]);
  check(objectiveFrame.length==512&&RoomWire.decode(objectiveFrame).type==17);
  rejects(()->RoomWire.frame(RoomWire.OBJECTIVE_CHUNK,new byte[505]));
  rejects(()->RoomWire.frame(18,new byte[0]));
  byte[] futureType=objectiveFrame.clone();futureType[5]=18;
  rejects(()->RoomWire.decode(futureType));
  rejects(()->RoomWire.decode(Arrays.copyOf(frame,frame.length+1)));
  rejects(()->RoomWire.join(new RoomWire.Join("bad\nname",nonce)));
  rejects(()->RoomWire.join(new RoomWire.Join("\ud800",nonce)));
  rejects(()->RoomWire.join(new RoomWire.Join("a".repeat(33),nonce)));
  byte[] badUtf8=frame.clone();badUtf8[8+20+2+16+1]=(byte)255;
  rejects(()->RoomWire.parseJoin(RoomWire.decode(badUtf8)));
  byte[] badImage=frame.clone();badImage[8]^=1;
  rejects(()->RoomWire.parseJoin(RoomWire.decode(badImage)));
  var roster=List.of(new RoomWire.Player(1,"Host"),new RoomWire.Player(2,"علي"));
  byte[] state=RoomWire.state(RoomWire.ACCEPT,nonce,nonce,2,1,"Room",roster);
  var parsed=RoomWire.parseState(RoomWire.decode(state));
  check(parsed.self==2&&parsed.players.size()==2&&parsed.players.get(1).name.equals("علي"));
  rejects(()->RoomWire.state(RoomWire.ACCEPT,nonce,nonce,3,1,"Room",roster));
  rejects(()->RoomWire.state(RoomWire.ACCEPT,nonce,nonce,2,1,"Room",List.of(new RoomWire.Player(1,"H"),new RoomWire.Player(1,"X"))));
  var stream=new ByteArrayOutputStream();RoomWire.write(new DataOutputStream(stream),frame);
  check(Arrays.equals(frame,RoomWire.read(new DataInputStream(new ByteArrayInputStream(stream.toByteArray())))));
  rejects(()->RoomWire.read(new DataInputStream(new ByteArrayInputStream(new byte[]{127,-1}))));
  System.out.println("PASS strict room wire, UTF-8, roster, framed I/O and objective profile 10 compatibility");
 }
}
