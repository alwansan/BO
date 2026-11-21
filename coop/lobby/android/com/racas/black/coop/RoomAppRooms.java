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
