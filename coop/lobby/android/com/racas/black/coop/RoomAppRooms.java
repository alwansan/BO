// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import android.content.Context;
import java.net.InetAddress;
import java.security.SecureRandom;
/** Process-owned room and nearby-device presence. No game actor or guest pointer. */
public final class RoomAppRooms {
 private static RoomSession active;private static RoomPresence.Service presence;private static boolean starting;private static long epoch;
 private static final byte[] id=new byte[16];private static volatile String name="Player";private static Context application;
 static{new SecureRandom().nextBytes(id);}
 private RoomAppRooms(){}
 public static synchronized RoomSession obtain(){if(active==null){active=RoomSession.create();active.setReporter(RoomDiagnostics::event);}return active;}
 public static byte[] presenceId(){return id.clone();}
 public static String displayName(){return name;}
 public static void setDisplayName(String value){
  try{String normalized=RoomWire.name(value);name=normalized;Context app=application;if(app!=null)app.getSharedPreferences("coop_room",Context.MODE_PRIVATE).edit().putString("name",normalized).apply();}catch(java.io.IOException ignored){}
 }
 public static void attach(Context context){
  // Initialize safe event persistence at process attach, not only on opening
  // the UI; room reconnection/errors must survive a dismissed dialog.
  RoomDiagnostics.init(context.getApplicationContext());
  final long token;final RoomSession session;
  synchronized(RoomAppRooms.class){
   application=context.getApplicationContext();
   if(presence!=null||starting)return;
   String model=android.os.Build.MODEL;try{RoomWire.name(model);}catch(Exception e){model="Player";}
   name=application.getSharedPreferences("coop_room",Context.MODE_PRIVATE).getString("name",model);
   session=obtain();starting=true;token=++epoch;
  }
  Thread worker=new Thread(()->{
   RoomPresence.Service candidate=null;
   try{candidate=RoomPresence.Service.start(InetAddress.getByAddress(new byte[]{0,0,0,0}),RoomPresence.PORT,id,()->{
    RoomSession.Status state=session.snapshot();
    if(state.phase==RoomSession.Phase.HOSTING&&state.roster!=null)return RoomPresence.Info.host(name,state.roster,state.port,RoomDirect.fingerprint(state.fingerprint));
    if(state.phase==RoomSession.Phase.JOINED&&state.roster!=null)return RoomPresence.Info.joined(name,state.roster);
    return RoomPresence.Info.idle(name);
   });
    synchronized(RoomAppRooms.class){if(token==epoch){presence=candidate;candidate=null;RoomDiagnostics.event("PRESENCE_READY");}}
   }catch(Exception e){RoomDiagnostics.event("PRESENCE_UNAVAILABLE");}
   finally{if(candidate!=null)candidate.close();synchronized(RoomAppRooms.class){if(token==epoch)starting=false;}}
  },"bo-presence-init");worker.setDaemon(true);worker.start();
 }
 private static final java.util.concurrent.atomic.AtomicBoolean localEyeLogged=
     new java.util.concurrent.atomic.AtomicBoolean();
 /**
  * Native simulation-thread hook only. Do not log coordinates or create
  * remote players. This is original local CAMERA eye, not actor body root.
  */
 public static void originalPlayerEyeFromGameThread(int x,int y,int z){
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session!=null&&session.observeOriginalEyeFromGameThread(x,y,z)&&
     localEyeLogged.compareAndSet(false,true))
   RoomDiagnostics.event("ORIGINAL_LOCAL_CAMERA_EYE_VALIDATED");
 }
 /** Only called from real original player hook after two known file opens. */
 public static void originalLevelResourceHintFromGameThread(int folder){
  if(folder!=0&&folder!=1&&(folder<3||folder>8))return;
  RoomSession current;
  synchronized(RoomAppRooms.class){current=active;}
  if(current!=null&&current.observeStageResourceFolderFromOriginalGameThread(folder))
   RoomDiagnostics.event("ORIGINAL_LEVEL_"+folder+"_TWO_FILES_OPEN_NOT_SCENE");
 }
 /**
  * A, B, C are ONLY UNPROVEN read-only XBE global values captured at the
  * original mission launch, then correlated with later actual level file
  * opens on this original player's game thread. Not a real mission index.
  */
 public static void originalMissionFieldCandidateFromGameThread(int code){
  if(code<100||code>342)return;
  final int encoded=code-100;
  final int which=encoded/81;
  final int candidateBucket=(encoded%81)/9;
  final int folder=encoded%9;
  if(which>2||candidateBucket>8||
     (folder!=0&&folder!=1&&(folder<3||folder>8)))return;
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session==null||session.originalStageFolderHintFromGameThread()!=folder)return;
  // No addresses, memory bytes, paths, credentials, names or arbitrary strings.
  final String source=which==0?"A":which==1?"B":"C";
  final String value=candidateBucket==0?"OTHER":Integer.toString(candidateBucket-1);
  RoomDiagnostics.event("ORIGINAL_MISSION_"+source+"_"+value+
                        "_LEVEL_"+folder+"_UNPROVEN");
 }
 public static void shutdown(){
  RoomSession old;RoomPresence.Service beacon;
  synchronized(RoomAppRooms.class){epoch++;starting=false;old=active;beacon=presence;active=null;presence=null;}
  if(beacon!=null)beacon.close();if(old!=null)old.close();
 }
 public static String statusLine(int row){
  RoomSession session;synchronized(RoomAppRooms.class){session=active;}
  RoomSession.Status state=session==null?null:session.snapshot();int count=state==null||state.roster==null?0:state.roster.players.size();
  switch(row){
   case -101:return state==null?"OFFLINE":state.phase==RoomSession.Phase.HOSTING?"HOSTING":state.phase==RoomSession.Phase.JOINED?"JOINED":state.phase==RoomSession.Phase.CONNECTING?"CONNECTING":state.phase==RoomSession.Phase.ERROR?"DISCONNECTED":"OFFLINE";
   case -102:return "HOST ROOM";case -103:return "NEARBY DEVICES";case -104:return count+"/4 HUMANS";case -105:return "LAN / VPN";default:return "";
  }
 }
}
