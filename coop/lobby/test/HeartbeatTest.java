package com.racas.black.coop;
import java.nio.file.*;
import java.net.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.atomic.*;
import javax.net.ssl.*;
public final class HeartbeatTest {
 public static void main(String[]args)throws Exception{
  Path identity=Paths.get(args[0]);InetAddress loop=InetAddress.getLoopbackAddress();
  AtomicReference<SSLSocket> accepted=new AtomicReference<>();
  try(SSLServerSocket server=(SSLServerSocket)TlsRoomTest.host(identity).getServerSocketFactory().createServerSocket(0,4,loop)){
   Thread fake=new Thread(()->{
    try(SSLSocket peer=(SSLSocket)server.accept()){
     accepted.set(peer);peer.setSoTimeout(3000);peer.startHandshake();
     var in=new DataInputStream(peer.getInputStream());var out=new DataOutputStream(peer.getOutputStream());
     var join=RoomWire.parseJoin(RoomWire.decode(RoomWire.read(in)));byte[] session=new byte[16];Arrays.fill(session,(byte)3);
     var people=List.of(new RoomWire.Player(1,"Host"),new RoomWire.Player(2,join.name));
     RoomWire.write(out,RoomWire.state(RoomWire.ACCEPT,session,join.nonce,2,1,"Room",people));
     long until=System.nanoTime()+15_000_000_000L;
     // A host can still send roster traffic while its input-processing path is dead.
     while(System.nanoTime()<until){RoomWire.write(out,RoomWire.state(RoomWire.ROSTER,session,join.nonce,2,1,"Room",people));Thread.sleep(250);}
    }catch(Exception expectedDisconnect){}
   },"test-host-without-pong");fake.setDaemon(true);fake.start();
   try(RoomTls.Client client=RoomTls.Client.connect(RoomTls.pinnedContext(TlsRoomTest.fingerprint(identity)),loop,server.getLocalPort(),"Client")){
    TlsRoomTest.await(()->!client.isRunning());
    TlsRoomTest.check(client.snapshot()==null);
   }finally{SSLSocket peer=accepted.get();if(peer!=null)peer.close();fake.join(2000);TlsRoomTest.check(!fake.isAlive());}
  }
  System.out.println("PASS heartbeat timeout despite continuing roster traffic");
 }
}
