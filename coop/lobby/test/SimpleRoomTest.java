package com.racas.black.coop;
import java.net.*;
import java.nio.file.*;
public final class SimpleRoomTest {
 public static void main(String[]args)throws Exception{
  InetAddress loop=InetAddress.getByName("127.0.0.1");
  try(RoomTls.Server host=RoomTls.Server.start(TlsRoomTest.host(Paths.get(args[0])),loop,0,"Host","Friends");
      RoomTls.Client peer=RoomDirect.joinAutomatic("127.0.0.1:"+host.port(),"Guest")){
   TlsRoomTest.await(()->host.snapshot().players.size()==2);
   TlsRoomTest.check(peer.snapshot().players.size()==2);
  }
  TlsRoomTest.check(RoomDirect.manual("192.168.0.145").port==24872);
  System.out.println("SIMPLE_ROOM_PASS: names-only host and automatic TLS join without password, hash entry or port entry");
 }
}
