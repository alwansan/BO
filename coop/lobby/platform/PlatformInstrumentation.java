// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import android.app.Activity;
import android.app.Instrumentation;
import android.os.Bundle;
import java.net.InetAddress;
import java.security.MessageDigest;

/** Standalone emulator test; no game, XBE, assets, JNI, or native library. */
public final class PlatformInstrumentation extends Instrumentation {
    private static void check(boolean value){if(!value)throw new AssertionError("Platform invariant failed");}
    @Override public void onCreate(Bundle arguments){super.onCreate(arguments);start();}
    @Override public void onStart(){
        Bundle result=new Bundle();
        try{
            RoomIdentity first=RoomIdentity.load(),second=RoomIdentity.load();
            check(first.fingerprint().length==32&&first.fingerprintHex().length()==64);
            check(MessageDigest.isEqual(first.fingerprint(),second.fingerprint()));
            InetAddress loop=InetAddress.getByName("127.0.0.1");
            try(RoomTls.Server host=RoomTls.Server.start(first.serverContext(),loop,0,"Host","Android Room","01234567")){
                try(RoomTls.Client peer=RoomTls.Client.connect(RoomTls.pinnedContext(first.fingerprint()),loop,host.port(),"Client","01234567")){
                    check(peer.snapshot()!=null&&peer.snapshot().players.size()==2&&peer.snapshot().self>1);
                    check(host.snapshot().players.size()==2);Thread.sleep(1500);check(peer.isRunning());
                }catch(Throwable failure){
                    Thread.sleep(80);
                    android.util.Log.e("BlackTlsPlatform","TLS server failure kind="+host.handshakeFailureKind()+
                                       " requestedKeyType="+first.lastRequestedKeyType());
                    throw failure;
                }
            }
            // Exercise the same bounded session lifecycle on a real Android
            // socket provider. This remains independent of the game's Flash/UI.
            try(RoomSession hosting=RoomSession.create();RoomSession joining=RoomSession.create()){
                hosting.startHost(()->new RoomSession.HostIdentity(
                    first.serverContext(),first.fingerprint()),loop,0,
                    "Phone Host","Android Room","01234567");
                long until=System.nanoTime()+7_000_000_000L;
                while(hosting.snapshot().phase!=RoomSession.Phase.HOSTING) {
                    check(System.nanoTime()<until);Thread.sleep(20);
                }
                RoomSession.Status info=hosting.snapshot();
                joining.join("127.0.0.1:"+info.port,info.fingerprint,"Phone Client","01234567");
                until=System.nanoTime()+7_000_000_000L;
                while(joining.snapshot().phase!=RoomSession.Phase.JOINED ||
                      hosting.snapshot().roster.players.size()!=2) {
                    check(System.nanoTime()<until);Thread.sleep(20);
                }
                joining.stop();hosting.stop();
                check(joining.snapshot().phase==RoomSession.Phase.IDLE);
            }
            long deadline=System.nanoTime()+8_000_000_000L;
            while(Thread.getAllStackTraces().keySet().stream().anyMatch(t->t.isAlive()&&t.getName().startsWith("bo-room-"))){
                if(System.nanoTime()>deadline)throw new AssertionError("Room threads survived close");Thread.sleep(10);
            }
            result.putString("stream","TLS_PLATFORM_PASS: opaque AndroidKeyStore identity persisted; pinned TLS host/client, roster, heartbeat and shutdown succeeded\n");
            finish(Activity.RESULT_OK,result);
        }catch(Throwable failure){
            android.util.Log.e("BlackTlsPlatform","Platform test failed",failure);
            result.putString("stream","TLS_PLATFORM_FAIL: "+failure.getClass().getName()+": "+failure.getMessage()+"\n");
            finish(Activity.RESULT_CANCELED,result);
        }
    }
}
