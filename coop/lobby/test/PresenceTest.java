package com.racas.black.coop;
import java.net.*;import java.nio.file.*;import java.util.*;import java.util.concurrent.atomic.*;
public final class PresenceTest {
 public static void main(String[]args)throws Exception{
  InetAddress a=InetAddress.getByName("127.0.0.1"),b=InetAddress.getByName("127.0.0.2");
  byte[] own=new byte[16],other=new byte[16];Arrays.fill(own,(byte)1);Arrays.fill(other,(byte)2);
  AtomicReference<RoomPresence.Provider> state=new AtomicReference<>(()->RoomPresence.Info.idle("Phone B"));
  try(RoomPresence.Service first=RoomPresence.Service.start(a,0,own,()->RoomPresence.Info.idle("Phone A"));
      RoomPresence.Service second=RoomPresence.Service.start(b,first.port(),other,()->state.get().get())){
   TlsRoomTest.check(second.port()==first.port());
   var peers=RoomPresence.scan(List.of(a,b),first.port(),500,own,new AtomicBoolean());
   TlsRoomTest.check(peers.size()==1&&peers.get(0).info.role==0&&peers.get(0).info.name.equals("Phone B"));
   try(RoomTls.Server host=RoomTls.Server.start(TlsRoomTest.host(Paths.get(args[0])),b,0,"B","Friends")){
    state.set(()->RoomPresence.Info.host("Phone B",host.snapshot(),host.port(),TlsRoomTest.fingerprint(Paths.get(args[0]))));
    peers=RoomPresence.scan(List.of(a,b),first.port(),700,own,new AtomicBoolean());
    TlsRoomTest.check(peers.size()==1&&peers.get(0).info.role==1&&peers.get(0).info.port==host.port());
    try(RoomTls.Client client=RoomDirect.joinAutomatic(b.getHostAddress()+":"+host.port(),"Guest")){
     TlsRoomTest.check(client.isRunning());
     peers=RoomPresence.scan(List.of(a,b),first.port(),700,own,new AtomicBoolean());
     TlsRoomTest.check(peers.get(0).info.occupied==2);
    }
   }
  }
  System.out.println("PRESENCE_PASS: running idle devices discovered, self excluded, live host port and real roster reflected");
 }
}
