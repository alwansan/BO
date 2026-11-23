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
 public static void originalPlayerEyeFromGameThread(int x,int y,int z,
                                                     int bx,int by,int bz,int sourceYaw){
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session!=null&&session.observeOriginalEyeFromGameThread(x,y,z,bx,by,bz,sourceYaw)&&
     localEyeLogged.compareAndSet(false,true))
   RoomDiagnostics.event("ORIGINAL_LOCAL_CAMERA_EYE_AND_BODY_FEET_VALIDATED");
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
  if(code<100||code>423)return;
  final int encoded=code-100;
  final int which=encoded/81;
  final int candidateBucket=(encoded%81)/9;
  final int folder=encoded%9;
  if(which>3||candidateBucket>8||
     (folder!=0&&folder!=1&&(folder<3||folder>8)))return;
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session==null||session.originalStageFolderHintFromGameThread()!=folder)return;
  // No addresses, memory bytes, paths, credentials, names or arbitrary strings.
  final String source=which==0?"A":which==1?"B_BEFORE":which==2?"B_AFTER":"C";
  final String value=candidateBucket==0?"OTHER":Integer.toString(candidateBucket-1);
  RoomDiagnostics.event("ORIGINAL_MISSION_"+source+"_"+value+
                        "_LEVEL_"+folder+"_UNPROVEN");
 }
 /**
  * Read-only, source XBE-confirmed original streaming candidate LEVEL/STAGE
  * values; only after matching original level file pair and stable game-thread
  * original player. NOT scene loaded or mission/ticket identity.
  */
 // Matched original core/stream identity only; cannot assert scene READY.
 // Host TLS roster ordinal slots only; original game thread guard is in
 // RoomSession. Returning 0 is NOT a scene-loaded or actor failure.
 public static int hostAppearancePeerMaskFromGameThread(){
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  return session==null?0:session.hostAppearancePeerMaskFromGameThread();
 }
 /**
  * Both original frontends: exact host-authenticated FIRST STAGE start,
  * no game engine writes/actor creation on this Java network boundary.
  */
 public static int[] firstStageAutoMenuPlanFromOriginalFrontend(){
  RoomSession s;
  synchronized(RoomAppRooms.class){s=active;}
  return s==null?new int[8]:s.firstStageAutoMenuPlanFromOriginalFrontend();
 }
 /** Poll from the game's original FRONTEND tick, even before a player
  * actor exists. Network-selected values ONLY, not a launcher call.
  */
 public static int[] guestPreparedStoryFromOriginalFrontend(){
  final RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  return session==null?new int[8]:
      session.guestPreparedStoryFromOriginalFrontend();
 }
 /** Called only from the stable ORIGINAL first-person player update, not
  * from lobby UI or network workers. Local level running is NOT a
  * synchronized SHARED AI world; READY applies to stage loading alone.
  */
 public static void originalRunningSceneFromGameThread(int episode,int level,
   int stage,int epoch,int revision,int tokenHi,int tokenLo,int slot){
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session==null)return;
  final int ready=session.attestOriginalRunningSceneFromGameThread(
      episode,level,stage,epoch,revision,tokenHi,tokenLo,slot);
  if(ready==1)
   RoomDiagnostics.event("ORIGINAL_HOST_RUNNING_LOCAL_SCENE_TLS_READY_NO_SHARED_AI");
  else if(ready==2)
   RoomDiagnostics.event("ORIGINAL_GUEST_RUNNING_LOCAL_SCENE_TLS_READY_NO_SHARED_AI");
 }
 public static void originalStageIdentityFromGameThread(int episode,int level,int stage){
  if(episode<=0||level<0||level>8||stage<0||stage>15)return;
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session!=null){
   if(session.observeOriginalStageIdentityFromGameThread(episode,level,stage))
    RoomDiagnostics.event("ORIGINAL_CORE_STREAM_STAGE_IDENTITY_CACHED_NOT_SCENE_READY");
   if(session.prepareObservedHostStageFromGameThread())
    RoomDiagnostics.event("ORIGINAL_HOST_STAGE_TLS_PREPARE_QUEUED_NOT_SCENE_READY");
  }
 }
 public static void originalStreamingStageFromGameThread(int code){
  if(code<500||code>2029)return;
  final int encoded=code-500, pair=encoded/9, folder=encoded%9;
  final int levelBucket=pair/17, stageBucket=pair%17;
  if(levelBucket>9||stageBucket>16||
     (folder!=0&&folder!=1&&(folder<3||folder>8)))return;
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session==null||session.originalStageFolderHintFromGameThread()!=folder)return;
  final String level=levelBucket==0?"OTHER":Integer.toString(levelBucket-1);
  final String stage=stageBucket==0?"OTHER":Integer.toString(stageBucket-1);
  RoomDiagnostics.event("ORIGINAL_STREAM_LEVEL_"+level+"_STAGE_"+stage+
                        "_FOLDER_"+folder+"_UNPROVEN");
 }
 /**
  * TRUE TLS peer-identity-validated HUMAN INPUT drained on the actual BLACK
  * original game thread only, after a real story COMMIT.
  * Shape: [sceneEpoch, count, [trustedPeer,sequence,moveX,moveY,yaw,fire|jump]*].
  * The TLS server enforces identity, ticket, revision, sequence and 250ms
  * freshness before creating TrustedInput. The native side ALSO validates.
  * Returns [0,0] to immediately purge native state when story is not live.
  * This is NOT a BLACK actor controller, renderer or NPC AI.
  */
 public static int[] pollTrustedHumanInputsFromGameThread(){
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session==null)return new int[]{0,0};
  RoomSession.StoryStatus before=session.storyStatus();
  if(!before.committed||before.ticket==null)return new int[]{0,0};
  java.util.List<RoomTls.Server.TrustedInput> source=
      session.drainHumanInputsFromGameThread();
  RoomSession.StoryStatus after=session.storyStatus();
  if(!after.committed||after.ticket==null||!before.ticket.same(after.ticket)||
     source.size()>3)return new int[]{0,0};
  java.util.ArrayList<RoomTls.Server.TrustedInput> items=
      new java.util.ArrayList<>(source);
  items.sort((a,b)->Integer.compare(a.playerSlot,b.playerSlot));
  int[] frame=new int[2+items.size()*6];
  frame[0]=after.ticket.sceneEpoch;
  frame[1]=items.size();
  int last=1;
  for(int i=0;i<items.size();i++){
   RoomTls.Server.TrustedInput trusted=items.get(i);
   RoomHumanInputWire.Input input=trusted.value;
   if(trusted.playerSlot<=last||trusted.playerSlot>4||input==null||
      !input.matches(after.ticket)||input.sequence<=0)return new int[]{0,0};
   int at=2+i*6;
   frame[at]=trusted.playerSlot;frame[at+1]=input.sequence;
   frame[at+2]=input.moveX;frame[at+3]=input.moveY;
   frame[at+4]=input.yaw;
   frame[at+5]=(input.fire?1:0)|(input.jump?2:0);
   last=trusted.playerSlot;
  }
  return frame;
 }
 /**
  * GUEST only: authenticated HOST world -> original BLACK game thread.
  * Strict story COMMIT, identity, roster and source tick agreement. Returns
  * [0,0,0,0] to purge native poses on scene/connection loss.
  * No app UI, network thread, original NPC/actor pointer or game write here.
  */
 public static int[] pollHostWorldSnapshotFromGameThread(){
  final int[] empty=new int[]{0,0,0,0};
  RoomSession session;
  synchronized(RoomAppRooms.class){session=active;}
  if(session==null)return empty;
  RoomSession.StoryStatus before=session.storyStatus();
  if(!before.committed||before.ticket==null)return empty;
  RoomWorldSnapshotWire.Snapshot source=session.readHostWorldFromGameThread();
  RoomSession.Status status=session.snapshot();
  RoomSession.StoryStatus after=session.storyStatus();
  if(!after.committed||after.ticket==null||
     !before.ticket.same(after.ticket)||source==null||
     status.phase!=RoomSession.Phase.JOINED||status.roster==null)
   return empty;
  try{return RoomGuestHostWorldSource.pack(after.ticket,source,status.roster);}
  catch(java.io.IOException invalidHostWorld){return empty;}
 }
 /**
  * Strict source-only HOST-authoritative two-phone other-human pose.
  * Must be invoked by the original BLACK game thread, never UI/TLS.
  * No original game memory, actors or AI can be changed by this function.
  */
 public static int[] pollOtherHumanBodySourceFromGameThread(){
  RoomSession s;
  synchronized(RoomAppRooms.class){s=active;}
  return s==null?new int[10]:s.otherHumanBodySourceFromGameThread();
 }
 /** Current HOST-authoritative bounded other-human BOT LOOK bodies. */
 public static int[] pollAllOtherHumanBodiesFromGameThread(){
  RoomSession s;
  synchronized(RoomAppRooms.class){s=active;}
  return s==null?new int[5]:s.allOtherHumanBodiesFromGameThread();
 }
 /**
  * ORIGINAL host scene, actual character-pool read-only values, 5Hz max.
  * Native supplies [originalEpisode,count,(hostSlot,xyzCm,hpCenti,dead)*].
  * NEVER accepts NPC positions from the remote player or menu UI.
  */
 public static void originalHostNpcSampleFromGameThread(int[] sample){
  if(sample==null||sample.length<2||sample.length>194||
     sample[0]<=0||sample[1]<0||sample[1]>32||
     sample.length!=2+6*sample[1])return;
  RoomSession s;
  synchronized(RoomAppRooms.class){s=active;}
  if(s==null)return;
  java.util.ArrayList<RoomNpcWorldWire.Npc> actors=
      new java.util.ArrayList<>(sample[1]);
  int last=-1;
  try{
   for(int i=0;i<sample[1];i++){
    int at=2+i*6,slot=sample[at];
    if(slot<=last)return;
    actors.add(new RoomNpcWorldWire.Npc(slot,sample[at+1],
      sample[at+2],sample[at+3],sample[at+4],sample[at+5]));
    last=slot;
   }
  }catch(java.io.IOException invalid){return;}
  if(s.publishOriginalHostNpcWorldFromGameThread(sample[0],actors)&&
     npcFirstSent.compareAndSet(false,true))
   RoomDiagnostics.event("ORIGINAL_HOST_NPC_VALUES_FIRST_TLS_QUEUED_NO_SHARED_AI");
 }
 private static final java.util.concurrent.atomic.AtomicBoolean npcFirstSent=
     new java.util.concurrent.atomic.AtomicBoolean();
 /**
  * Guest reads HOST-owned canonical NPC VALUES, never local game memory.
  * Does not freeze or overwrite guest NPC AI: that engine bridge is P0.
  */
 public static int[] pollHostOriginalNpcWorldFromGameThread(){
  RoomSession s;
  synchronized(RoomAppRooms.class){s=active;}
  if(s==null)return new int[3];
  RoomNpcWorldWire.World w=s.readOriginalHostNpcWorldFromGameThread();
  if(w==null)return new int[3];
  int[] answer=new int[3+w.npcs.size()*6];
  answer[0]=w.epoch;answer[1]=w.tick;answer[2]=w.npcs.size();
  for(int i=0;i<w.npcs.size();i++){
   RoomNpcWorldWire.Npc n=w.npcs.get(i);int at=3+i*6;
   answer[at]=n.slot;answer[at+1]=n.xCm;answer[at+2]=n.yCm;
   answer[at+3]=n.zCm;answer[at+4]=n.healthCenti;
   answer[at+5]=n.flags;
  }
  return answer;
 }
 /**
  * One HOST-only event to verified ORIGINAL guest game thread.
  * Source values only; no game memory, bot AI, or object mutation.
  */
 public static int[] pollHostObjectEventFromGameThread(){
  RoomSession s;
  synchronized(RoomAppRooms.class){s=active;}
  return s==null?new int[]{-1}:
      s.pollOriginalHostObjectEventFrameFromGameThread();
 }
 // Source-only native gamepad -> authenticated GUEST TLS inputs.
 // Exact COMMITTED ticket scope resets the per-story sequence.
 // Not a loaded scene, NPC input, remote actor controller or physics.
 private static RoomSession padSession;
 private static RoomStoryWire.Ticket padTicket;
 private static int padSequence;
 private static final java.util.concurrent.atomic.AtomicBoolean padErrorLogged=
     new java.util.concurrent.atomic.AtomicBoolean();
 public static boolean offerOriginalGuestControllerFromGameThread(
    int x,int y,int yaw,boolean fire,boolean jump){
  if(x< -1000||x>1000||y< -1000||y>1000||
     yaw< -18000||yaw>18000)return false;
  final RoomSession session;
  final int sequence;
  synchronized(RoomAppRooms.class){
   session=active;
   if(session==null)return false;
   RoomSession.Status roster=session.snapshot();
   if(roster.phase!=RoomSession.Phase.JOINED)return false;
   RoomSession.StoryStatus story=session.storyStatus();
   if(!story.committed||story.ticket==null)return false;
   if(padSession!=session||padTicket==null||!padTicket.same(story.ticket)){
    padSession=session;padTicket=story.ticket;padSequence=0;
    padErrorLogged.set(false);
   }
   if(padSequence==Integer.MAX_VALUE)return false;
   sequence=++padSequence;
  }
  try {
   return session.offerHumanInputFromGameThread(sequence,x,y,yaw,fire,jump);
  }catch(java.io.IOException rejected){
   if(padErrorLogged.compareAndSet(false,true))
    RoomDiagnostics.event("ORIGINAL_GUEST_GAMEPAD_TLS_SEND_REJECTED");
   return false;
  }
 }
 public static void shutdown(){
  RoomSession old;RoomPresence.Service beacon;
  synchronized(RoomAppRooms.class){epoch++;starting=false;old=active;beacon=presence;active=null;presence=null;padSession=null;padTicket=null;padSequence=0;}
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
