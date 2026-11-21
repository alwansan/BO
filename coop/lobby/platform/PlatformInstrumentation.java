// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import android.app.Activity;
import android.app.Instrumentation;
import android.content.Intent;
import android.view.KeyEvent;
import android.os.Bundle;
import java.net.InetAddress;
import java.security.MessageDigest;

/** Standalone emulator test; no game, XBE, assets, JNI, or native library. */
public final class PlatformInstrumentation extends Instrumentation {
    private static void check(boolean value){if(!value)throw new AssertionError("Platform invariant failed");}
    private void verifySafeRotatedRoomLogs()throws Exception{
        android.content.Context context=getTargetContext().getApplicationContext();
        RoomDiagnostics.init(context);
        java.io.File base=context.getExternalFilesDir(null);
        if(base==null)base=context.getFilesDir();
        java.io.File dir=new java.io.File(base,"logs");
        check(dir.isDirectory()||dir.mkdirs());
        java.io.File recent=new java.io.File(dir,"coop_room_events.log");
        java.io.File older=new java.io.File(dir,"coop_room_events.old.log");
        check(!recent.exists()||recent.delete());
        check(!older.exists()||older.delete());
        RoomDiagnostics.event("STORY_DIAGNOSTICS_BEGIN");
        for(int i=0;i<900;i++)
            RoomDiagnostics.event("STORY_SAFE_ROTATION_SAMPLE_ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789");
        RoomDiagnostics.event("STORY_DIAGNOSTICS_END");
        check(recent.isFile()&&older.isFile());
        String safe=RoomDiagnostics.read();
        check(safe.contains("STORY_DIAGNOSTICS_BEGIN")&&safe.contains("STORY_DIAGNOSTICS_END"));
        check(RoomDiagnostics.exportToDownloads(context));
        try(java.io.FileOutputStream write=new java.io.FileOutputStream(older,true)){
            write.write("UNTRUSTED_RAW_ADDRESS_OR_SECRET\n".getBytes(java.nio.charset.StandardCharsets.UTF_8));
        }
        check(RoomDiagnostics.read().startsWith("Unable to read"));
        check(recent.delete()&&older.delete());
        RoomDiagnostics.event("ROOM_SAFE_LOG_EXPORT_VERIFIED");
    }
    @Override public void onCreate(Bundle arguments){super.onCreate(arguments);start();}
    @Override public void onStart(){
        Bundle result=new Bundle();
        try{
            verifySafeRotatedRoomLogs();
            RoomIdentity first=RoomIdentity.load(),second=RoomIdentity.load();
            check(first.fingerprint().length==32&&first.fingerprintHex().length()==64);
            check(MessageDigest.isEqual(first.fingerprint(),second.fingerprint()));
            InetAddress loop=InetAddress.getByName("127.0.0.1");
            try(RoomTls.Server host=RoomTls.Server.start(first.serverContext(),loop,0,"Host","Android Room")){
                try(RoomTls.Client peer=RoomTls.Client.connect(RoomTls.pinnedContext(first.fingerprint()),loop,host.port(),"Client")){
                    check(peer.snapshot()!=null&&peer.snapshot().players.size()==2&&peer.snapshot().self>1);
                    check(host.snapshot().players.size()==2);Thread.sleep(1500);check(peer.isRunning());
                    // Real Conscrypt/AndroidKeyStore encrypted story envelopes:
                    // no BLACK scene loader or gameplay is invoked.
                    RoomStoryWire.Ticket ticket=host.prepareStory(0,0,1);
                    long storyDeadline=System.nanoTime()+8_000_000_000L;
                    while(peer.storyTicket()==null){
                        check(System.nanoTime()<storyDeadline);Thread.sleep(15);
                    }
                    check(ticket.same(peer.storyTicket()));
                    check(!peer.acknowledgeSceneLoaded(0,1,1)); // wrong stage
                    check(!host.hostSceneLoaded(0,0,1)); // waits for guest
                    check(peer.acknowledgeSceneLoaded(0,0,1)); // mock loader proof for emulator only
                    while(!host.storyCommitted()||!peer.storyCommitted()){
                        check(System.nanoTime()<storyDeadline);Thread.sleep(15);
                    }
                    check(peer.isRunning());
                }catch(Throwable failure){
                    Thread.sleep(80);
                    android.util.Log.e("BlackTlsPlatform","TLS server failure kind="+host.handshakeFailureKind()+
                                       " requestedKeyType="+first.lastRequestedKeyType());
                    throw failure;
                }
            }
            // Owner reported BindException after quick Host/Leave/Host.
            // Exercise a closed listener's port with active TLS clients, while
            // rejecting another LISTEN before the old host closes.
            int restartPort;
            try(RoomTls.Server earlier=RoomTls.Server.start(first.serverContext(),loop,0,"Host","Rebind")){
                restartPort=earlier.port();
                try(RoomTls.Server duplicate=RoomTls.Server.start(first.serverContext(),loop,restartPort,"Other","Other")){
                    throw new AssertionError("Concurrent listener unexpectedly accepted port "+duplicate.port());
                }catch(java.net.BindException expected){}
                try(RoomTls.Client connection=RoomTls.Client.connect(
                    RoomTls.pinnedContext(first.fingerprint()),loop,restartPort,"Before restart")){
                    check(connection.snapshot().players.size()==2);
                }
            }
            for(int i=0;i<3;i++){
                try(RoomTls.Server again=RoomTls.Server.start(first.serverContext(),loop,restartPort,"Host","Rebind");
                    RoomTls.Client connection=RoomTls.Client.connect(
                        RoomTls.pinnedContext(first.fingerprint()),loop,restartPort,"Rejoined")){
                    check(again.port()==restartPort&&connection.snapshot().players.size()==2);
                }
            }
            // Exercise the same bounded session lifecycle on a real Android
            // socket provider. This remains independent of the game's Flash/UI.
            try(RoomSession hosting=RoomSession.create();RoomSession joining=RoomSession.create()){
                hosting.startHost(()->new RoomSession.HostIdentity(
                    first.serverContext(),first.fingerprint()),loop,0,
                    "Phone Host","Android Room");
                long until=System.nanoTime()+7_000_000_000L;
                while(hosting.snapshot().phase!=RoomSession.Phase.HOSTING) {
                    check(System.nanoTime()<until);Thread.sleep(20);
                }
                RoomSession.Status info=hosting.snapshot();
                joining.joinAutomatic("127.0.0.1:"+info.port,"Phone Client");
                until=System.nanoTime()+7_000_000_000L;
                while(joining.snapshot().phase!=RoomSession.Phase.JOINED ||
                      hosting.snapshot().roster.players.size()!=2) {
                    check(System.nanoTime()<until);Thread.sleep(20);
                }
                joining.stop();hosting.stop();
                check(joining.snapshot().phase==RoomSession.Phase.IDLE);
            }
            // Verify the actual Android Back key dismisses a same-Activity room dialog
            // without calling finish() on the underlying game-activity surrogate.
            Intent intent=new Intent(Intent.ACTION_MAIN);
            intent.setClassName(getTargetContext().getPackageName(),
                "com.racas.black.coop.RoomHarnessActivity");
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            Activity harness=startActivitySync(intent);
            check(harness!=null&&!harness.isFinishing());
            runOnMainSync(()->RoomOverlay.show(harness,-102));
            waitForIdleSync();
            runOnMainSync(()->RoomOverlay.startHostForTest());
            long hosted=System.nanoTime()+8_000_000_000L;
            while(RoomAppRooms.obtain().snapshot().phase!=RoomSession.Phase.HOSTING){
                check(System.nanoTime()<hosted);Thread.sleep(20);
            }
            check(RoomAppRooms.obtain().snapshot().port==RoomDirect.DEFAULT_PORT);
            // CL two-device evidence: repeated host taps must not collide with
            // an active listener; auto-discovery must stop once HOSTING/JOINED.
            runOnMainSync(()->RoomOverlay.startHostForTest());
            check(RoomAppRooms.obtain().snapshot().phase==RoomSession.Phase.HOSTING);
            check(RoomDiagnostics.read().contains("HOST_BLOCKED_ALREADY_ACTIVE"));
            int scansBefore=RoomDiagnostics.read().split(" SCAN_STARTED\\n",-1).length;
            runOnMainSync(()->RoomOverlay.show(harness,-103));
            Thread.sleep(6100); // longer than the 5.5s automatic discovery cadence
            waitForIdleSync();
            int scansAfter=RoomDiagnostics.read().split(" SCAN_STARTED\\n",-1).length;
            check(scansAfter==scansBefore);
            check(RoomAppRooms.obtain().snapshot().phase==RoomSession.Phase.HOSTING);
            waitForIdleSync();
            sendKeyDownUpSync(KeyEvent.KEYCODE_BACK);
            waitForIdleSync();
            check(!harness.isFinishing()&&!harness.isDestroyed());
            runOnMainSync(()->{RoomAppRooms.shutdown();harness.finish();});
            long deadline=System.nanoTime()+8_000_000_000L;
            while(Thread.getAllStackTraces().keySet().stream().anyMatch(t->t.isAlive()&&t.getName().startsWith("bo-room-"))){
                if(System.nanoTime()>deadline)throw new AssertionError("Room threads survived close");Thread.sleep(10);
            }
            result.putString("stream","TLS_PLATFORM_PASS: AndroidKeyStore TLS room, host-story PREPARE / guest explicit READY / host COMMIT and same-port rapid rehost succeeded over Android29 sockets; names-only Host, automatic Join and Back regression passed; ROOM_LOG_ROTATION_PASS verified safe rotated MediaStore export and tamper refusal; ROOM_ACTIVE_SCAN_GATE_PASS no scan storm or duplicate host while connected (NO BLACK game loader/actor)\n");
            finish(Activity.RESULT_OK,result);
        }catch(Throwable failure){
            android.util.Log.e("BlackTlsPlatform","Platform test failed",failure);
            result.putString("stream","TLS_PLATFORM_FAIL: "+failure.getClass().getName()+": "+failure.getMessage()+"\n");
            finish(Activity.RESULT_CANCELED,result);
        }
    }
}
