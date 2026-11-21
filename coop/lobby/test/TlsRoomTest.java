package com.racas.black.coop;
import java.net.*;
import java.nio.file.*;
import java.security.*;
import javax.net.ssl.*;
import java.util.*;
import java.util.concurrent.*;
public final class TlsRoomTest {
 interface Condition{boolean ok()throws Exception;}
 static void check(boolean v){if(!v)throw new AssertionError();}
 static void await(Condition c)throws Exception{long end=System.nanoTime()+8_000_000_000L;while(!c.ok()){if(System.nanoTime()>end)throw new AssertionError("timed out");Thread.sleep(10);}}
 static SSLContext host(Path p)throws Exception{
  KeyStore ks=KeyStore.getInstance("PKCS12");try(var in=Files.newInputStream(p)){ks.load(in,"test-only".toCharArray());}
  KeyManagerFactory km=KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());km.init(ks,"test-only".toCharArray());
  SSLContext c=SSLContext.getInstance("TLS");c.init(km.getKeyManagers(),null,new SecureRandom());return c;
 }
 static byte[] fingerprint(Path p)throws Exception{
  KeyStore ks=KeyStore.getInstance("PKCS12");try(var in=Files.newInputStream(p)){ks.load(in,"test-only".toCharArray());}
  return MessageDigest.getInstance("SHA-256").digest(ks.getCertificate("room").getEncoded());
 }
 public static void main(String[]args)throws Exception{
  InetAddress loop=InetAddress.getLoopbackAddress();Path identity=Paths.get(args[0]);byte[] fp=fingerprint(identity);
  SSLContext client=RoomTls.pinnedContext(fp);String pin="01234567";
  List<RoomTls.Client> clients=new ArrayList<>();
  try(RoomTls.Server server=RoomTls.Server.start(host(identity),loop,0,"Host","Real room",pin)){
   // A trickled first JOIN must have an absolute deadline, not a fresh timeout per byte.
   try(SSLSocket slow=(SSLSocket)client.getSocketFactory().createSocket(loop,server.port())){
    slow.setSoTimeout(6500);slow.startHandshake();
    var output=slow.getOutputStream();output.write(new byte[]{66,67,82,77,1,1,1,(byte)248});output.flush();
    var trickle=Executors.newSingleThreadScheduledExecutor();
    trickle.scheduleAtFixedRate(()->{try{output.write(0);output.flush();}catch(java.io.IOException ignored){}},0,1,TimeUnit.SECONDS);
    try{check(slow.getInputStream().read()==-1);}
    catch(SocketTimeoutException timeout){throw new AssertionError("Unauthenticated frame exceeded absolute deadline",timeout);}
    catch(javax.net.ssl.SSLException closedByHost){}
    finally{trickle.shutdownNow();}
   }
   // Exercise compatibility rejection and byte-fragmented TLS application records on real sockets.
   for(int mode=0;mode<3;mode++){
    try(SSLSocket raw=(SSLSocket)client.getSocketFactory().createSocket(loop,server.port())){
     raw.setSoTimeout(3000);raw.startHandshake();
     byte[] request=RoomWire.join(new RoomWire.Join("Fragment",pin,new byte[16]));
     if(mode==0)request[8]^=1;
     if(mode==1)request[4]=99;
     var output=raw.getOutputStream();
     if(mode==2){for(byte b:request){output.write(b);output.flush();}}
     else{output.write(request);output.flush();}
     var response=RoomWire.decode(RoomWire.read(new java.io.DataInputStream(raw.getInputStream())));
     if(mode<2)check(response.type==RoomWire.REJECT&&RoomWire.parseReject(response)==2);
     else check(response.type==RoomWire.ACCEPT&&RoomWire.parseState(response).self>1);
    }
    await(()->server.snapshot().players.size()==1);
   }
   try{RoomTls.Client.connect(client,loop,server.port(),"Wrong PIN","99999999");throw new AssertionError();}catch(RoomTls.Rejected e){check(e.reason==1);}
   check(server.snapshot().players.size()==1);
   byte[] wrong=fp.clone();wrong[0]^=1;
   try{RoomTls.Client.connect(RoomTls.pinnedContext(wrong),loop,server.port(),"Wrong identity",pin);throw new AssertionError();}catch(javax.net.ssl.SSLException expected){}
   for(String name:List.of("One","اثنان","Three"))clients.add(RoomTls.Client.connect(client,loop,server.port(),name,pin));
   await(()->server.snapshot().players.size()==4&&clients.stream().allMatch(c->c.snapshot()!=null&&c.snapshot().players.size()==4));
   var exposed=clients.get(0).snapshot();byte original=exposed.session[0];exposed.session[0]^=1;
   check(clients.get(0).snapshot().session[0]==original);
   Set<Integer> ids=new HashSet<>();for(var c:clients)check(ids.add(c.snapshot().self));check(!ids.contains(1));
   try{RoomTls.Client.connect(client,loop,server.port(),"Full",pin);throw new AssertionError();}catch(RoomTls.Rejected e){check(e.reason==3);}
   int old=clients.get(0).snapshot().self;
   var closing=clients.remove(0);var parsedBeforeClose=closing.snapshot();closing.close();
   // Deterministic interleaving: parse/validate, close, then delayed publication.
   closing.publish(parsedBeforeClose);check(closing.snapshot()==null);
   await(()->server.snapshot().players.size()==3);
   var replacement=RoomTls.Client.connect(client,loop,server.port(),"Replacement",pin);clients.add(replacement);check(replacement.snapshot().self!=old);
   await(()->clients.stream().allMatch(c->c.snapshot()!=null&&c.snapshot().players.size()==4));
   Thread.sleep(1500);check(clients.stream().allMatch(RoomTls.Client::isRunning));
  }finally{for(var c:clients)c.close();}
  await(()->Thread.getAllStackTraces().keySet().stream().noneMatch(t->t.isAlive()&&t.getName().startsWith("bo-room-")));
  System.out.println("PASS actual TLS sockets: pinned host, PIN rejection, four humans, full room, fresh IDs, disconnect, shutdown");
 }
}
