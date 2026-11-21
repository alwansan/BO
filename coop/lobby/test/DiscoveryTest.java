package com.racas.black.coop;
import java.net.*;
import java.nio.file.*;
import java.util.*;
import java.util.concurrent.atomic.*;
public final class DiscoveryTest {
 public static void main(String[]args)throws Exception{
  InetAddress loop=InetAddress.getByName("127.0.0.1");Path identity=Paths.get(args[0]);
  try(RoomTls.Server server=RoomTls.Server.start(TlsRoomTest.host(identity),loop,0,"Host","غرفة","01234567");
      RoomDiscovery.Host discovery=RoomDiscovery.Host.start(server,loop,0)){
   var found=RoomDiscovery.scan(List.of(loop),discovery.port(),1200,new AtomicBoolean());
   TlsRoomTest.check(found.size()==1&&found.get(0).port==server.port()&&found.get(0).occupied==1&&found.get(0).compatible&&found.get(0).room.equals("غرفة"));
   try(RoomTls.Client peer=RoomTls.Client.connect(RoomTls.pinnedContext(TlsRoomTest.fingerprint(identity)),loop,server.port(),"Client","01234567")){
    found=RoomDiscovery.scan(List.of(loop),discovery.port(),1200,new AtomicBoolean());
    TlsRoomTest.check(found.size()==1&&found.get(0).occupied==2);
    TlsRoomTest.check(peer.isRunning());
   }
   AtomicBoolean cancel=new AtomicBoolean(true);long before=System.nanoTime();
   TlsRoomTest.check(RoomDiscovery.scan(List.of(loop),discovery.port(),5000,cancel).isEmpty());
   TlsRoomTest.check(System.nanoTime()-before<500_000_000L);
  }
  byte[] reply=RoomDiscovery.reply(7,1508,1,"Room");
  var decoded=RoomDiscovery.parseReply(reply,loop);TlsRoomTest.check(decoded.compatible&&decoded.challenge==7);
  byte[] wrong=reply.clone();wrong[15+4]^=1;TlsRoomTest.check(!RoomDiscovery.parseReply(wrong,loop).compatible);
  try{RoomDiscovery.parseReply(Arrays.copyOf(reply,reply.length+1),loop);throw new AssertionError();}catch(java.io.IOException expected){}
  try{RoomDiscovery.query(0);throw new AssertionError();}catch(java.io.IOException expected){}
  System.out.println("PASS actual UDP discovery, real TLS roster counts, compatibility metadata, cancellation and cleanup");
 }
}
