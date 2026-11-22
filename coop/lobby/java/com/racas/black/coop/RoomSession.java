// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.IOException;
import java.net.ConnectException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import javax.net.ssl.SSLHandshakeException;
import java.net.InetAddress;
import java.security.GeneralSecurityException;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Executor;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import javax.net.ssl.SSLContext;

/**
 * Explicit, bounded owner-controlled lobby lifecycle. The worker never calls
 * the game, modifies guest memory or creates simulated players.
 */
public final class RoomSession implements AutoCloseable {
    public enum Phase { IDLE, CONNECTING, HOSTING, JOINED, ERROR }
    public static final class HostIdentity {
        public final SSLContext tls;
        public final byte[] fingerprint;
        public HostIdentity(SSLContext tls, byte[] fingerprint) {
            if(tls==null || fingerprint==null || fingerprint.length!=32)
                throw new IllegalArgumentException("Full host identity is required");
            this.tls=tls;this.fingerprint=fingerprint.clone();
        }
    }
    public interface IdentitySource { HostIdentity load() throws Exception; }
    public interface Reporter { void event(String safeCode); }
    private volatile Reporter reporter;
    public void setReporter(Reporter sink){reporter=sink;}
    private void event(String code){Reporter sink=reporter;if(sink!=null)try{sink.event(code);}catch(RuntimeException ignored){}}
    public static final class Status {
        public final Phase phase;
        public final String description, fingerprint;
        public final int port;
        public final RoomWire.State roster;
        private Status(Phase phase,String description,String fingerprint,int port,RoomWire.State roster) {
            this.phase=phase;this.description=description;this.fingerprint=fingerprint;
            this.port=port;this.roster=roster==null?null:RoomWire.copy(roster);
        }
    }
    private final Object lock=new Object();
    private final ThreadPoolExecutor work=new ThreadPoolExecutor(
        1,1,0L,TimeUnit.MILLISECONDS,new ArrayBlockingQueue<>(1),r->{
            Thread t=new Thread(r,"bo-lobby-session");t.setDaemon(true);return t;
        },new ThreadPoolExecutor.DiscardOldestPolicy());
    // Only a future VERIFIED BLACK engine callback may claim this thread.
    // This Java guard prevents socket/UI workers from accidentally invoking
    // the story APIs once the real game thread is bound. It is not proof of
    // a real game hook, scene loader, or shared simulation.
    private Thread storyGameThread;
    // Read-only original first-person CAMERA EYE; never a remote actor root.
    public static final class LocalEye {
        public final int xCm,yCm,zCm;
        private final long atNs;
        private LocalEye(int x,int y,int z,long at){
            xCm=x;yCm=y;zCm=z;atNs=at;
        }
    }
    private LocalEye lastEye;
    private int localEyeTransportSeq,hostEyeWorldTick;
    private boolean eyeBridgeErrorLogged;
    // HOST game-thread value-only remote HUMAN movement proxy.
    // Source: real BLACK actor feet; inputs: authenticated TLS socket peers.
    // This is not a real BLACK character, AI, collision or weapon simulation.
    private final RoomHostHumanKinematics hostHumanPoses=new RoomHostHumanKinematics();
    // ORIGINAL local stage candidate, NOT the host-owned TLS scene epoch.
    public static final class OriginalStageCandidate {
        public final int originalEpisode,level,stage;
        private OriginalStageCandidate(int e,int l,int s){
            originalEpisode=e;level=l;stage=s;
        }
    }
    private OriginalStageCandidate lastSourceStage;
    private long lastSourceStageAtNs;
    private final RoomOriginalHostStagePrepareGate sourceStagePrepare=new RoomOriginalHostStagePrepareGate();
    private Thread originalGuestFrontendThread;
    // Exact ticket identity for at-most-once local original-running-scene
    // confirmation; native source cannot authorize an unrelated TLS story.
    private RoomStoryWire.Ticket originalRunningReadyTicket;
    /** Compact TLS roster positions, NOT long-lived RoomWire.Player IDs.
     * Source-only bot appearance proposal for genuine joined human devices. */
    public int hostAppearancePeerMaskFromGameThread(){
        synchronized(lock){
            if(closed||!validStoryThread()||phase!=Phase.HOSTING||
               host==null||!host.isRunning())return 0;
            try{
                RoomWire.State state=host.snapshot();
                if(state==null||state.players==null||
                   state.players.size()<2||state.players.size()>4||
                   state.players.get(0).id!=1)return 0;
                int joined=state.players.size()-1;
                return ((1<<(joined+1))-1)&0x0E;
            }catch(IOException unavailable){
                return 0;
            }
        }
    }
    boolean observeOriginalStageIdentityFromGameThread(int episode,int level,int stage){
        synchronized(lock){
            if(closed||!validStoryThread()||
               (phase!=Phase.HOSTING&&phase!=Phase.JOINED)||
               episode<=0||level<0||level>8||stage<0||stage>15||
               (lastSourceStage!=null&&episode<lastSourceStage.originalEpisode))
                return false;
            long now=System.nanoTime();
            if(lastSourceStage!=null&&episode==lastSourceStage.originalEpisode){
                if(level!=lastSourceStage.level||stage!=lastSourceStage.stage)return false;
                lastSourceStageAtNs=now;
                if(phase==Phase.HOSTING)sourceStagePrepare.observe(episode,level,stage,now);
                return false;
            }
            // A NEW source-matched ORIGINAL campaign stage supersedes any
            // old host ticket. Abort old host input/poses/peer READY before
            // sending PREPARE for the next stage; no engine writes/loader.
            if(phase==Phase.HOSTING&&host!=null&&host.storyTicket()!=null){
                RoomStoryWire.Ticket pending=host.storyTicket();
                // A manually prepared first-stage ticket is invalid if the
                // actual host original engine loads a different mission.
                // Previously entered stages also revoke on fresh episode.
                if(pending.mission!=level||pending.stage!=stage||
                   (lastSourceStage!=null&&
                    episode>lastSourceStage.originalEpisode)){
                    host.cancelStory();
                    event("ORIGINAL_HOST_STAGE_MISMATCH_ABORTED");
                }
            }
            lastSourceStage=new OriginalStageCandidate(episode,level,stage);
            lastSourceStageAtNs=now;
            if(phase==Phase.HOSTING)sourceStagePrepare.observe(episode,level,stage,now);
            return true;
        }
    }
    /**
     * Host-owned TWO-PHONE FIRST STAGE START from the Android lobby.
     * Sends a genuine TLS STORY_PREPARE for level 0/stage 1 to the one
     * connected phone. The original game stage is NOT started here.
     * Original game-thread source stage matching and independent scene
     * readiness remain necessary for READY/COMMIT, with no fake actors.
     */
    public boolean requestTwoPhoneFirstStageFromLobby(){
        synchronized(lock){
            if(closed||phase!=Phase.HOSTING||host==null||
               !host.isRunning()||host.storyTicket()!=null)return false;
            try{
                RoomWire.State current=host.snapshot();
                if(current==null||current.players.size()!=2||
                   current.players.get(0).id!=1||
                   (lastSourceStage!=null&&
                    (lastSourceStage.level!=0||lastSourceStage.stage!=1)))
                    return false;
                int epoch=sourceStagePrepare.nextTwoPhoneFirstStageEpoch(
                    current.revision,1,false);
                if(epoch==0)return false;
                RoomStoryWire.Ticket ticket=host.prepareStory(0,1,epoch);
                if(ticket==null||ticket.mission!=0||ticket.stage!=1||
                   ticket.participants!=3||
                   ticket.rosterRevision!=current.revision||
                   !sourceStagePrepare.markTwoPhoneFirstStageOffered(
                       ticket.rosterRevision,epoch)){
                    host.cancelStory();return false;
                }
                event("TWO_PHONE_FIRST_STAGE_PREPARED_NEEDS_ORIGINAL_LAUNCH");
                return true;
            }catch(IOException unavailable){return false;}
        }
    }
    /** Native original core/stream match queues real TLS STORY_PREPARE only.
     * Does NOT start guests, trigger scene READY, or touch original game RAM.
     */
    public boolean prepareObservedHostStageFromGameThread(){
        synchronized(lock){
            if(!validStoryThread()||phase!=Phase.HOSTING||host==null||
               !host.isRunning()||lastSourceStage==null||
               host.storyTicket()!=null)return false;
            try{
                RoomWire.State state=host.snapshot();
                if(state.players.size()<2||state.players.size()>4||
                   state.players.get(0).id!=1)return false;
                int epoch=sourceStagePrepare.nextOffer(
                    state.revision,state.players.size()-1,false,System.nanoTime());
                if(epoch==0)return false;
                RoomStoryWire.Ticket ticket=host.prepareStory(
                    sourceStagePrepare.level(),sourceStagePrepare.stage(),epoch);
                // The server may have accepted a new peer between roster
                // snapshot and prepare. Mark the ACTUAL ticket revision.
                return ticket!=null&&sourceStagePrepare.markOffered(
                    ticket.rosterRevision,epoch);
            }catch(IOException ignored){return false;}
        }
    }
    /** Original native FRONTEND callback on guest: TLS-authenticated host
     * PREPARE values enter original engine thread even WITHOUT a loaded
     * player body. Does NOT call the BLACK stage loader or publish READY.
     */
    public int[] guestPreparedStoryFromOriginalFrontend(){
        synchronized(lock){
            final int[] empty=new int[8];
            if(closed||phase!=Phase.JOINED||client==null||
               !client.isRunning())return empty;
            if(originalGuestFrontendThread==null)
                originalGuestFrontendThread=Thread.currentThread();
            if(originalGuestFrontendThread!=Thread.currentThread())return empty;
            RoomStoryWire.Ticket ticket=client.storyTicket();
            RoomWire.State roster=client.snapshot();
            if(ticket==null||roster==null||client.storyCommitted()||
               ticket.mission<0||ticket.mission>7||
               ticket.stage<0||ticket.stage>15||
               roster.revision!=ticket.rosterRevision||
               !java.security.MessageDigest.isEqual(roster.session,ticket.session())||
               roster.players.size()<2||roster.players.size()>4||
               roster.players.size()!=Integer.bitCount(ticket.participants))
                return empty;
            int slot=RoomWire.storySlot(roster,roster.self);
            if(slot<2||slot>4||!ticket.includes(slot))return empty;
            return new int[]{ticket.sceneEpoch,ticket.mission,ticket.stage,
                (int)ticket.rosterRevision,(int)(ticket.token>>>32),
                (int)ticket.token,slot,ticket.participants};
        }
    }
    /**
     * A second 30 stable original BLACK player/world samples (on top of
     * the native source-matched core/stream/phase/resources) confirm this
     * phone's ORIGINAL level is RUNNING and equals the pending HOST ticket.
     *
     * Host: local scene-ready barrier bit. Guest: enqueue TLS READY only
     * if SAME authenticated ticket was present BEFORE original guest's
     * genuine frontend launch, original streaming stage and running-world
     * witness. NO auto guest level launch or original game memory writes.
     * Local matching stages do NOT mean synchronized AI, actors or quests.
     *
     * Guest tokenHi/tokenLo are bit-exact split of the POSITIVE TLS ticket
     * token; host ignores them and requires no guest look/selection key.
     */
    public int attestOriginalRunningSceneFromGameThread(int episode,
         int level,int stage,int sceneEpoch,int rosterRevision,
         int tokenHi,int tokenLo,int guestSlot){
        synchronized(lock){
            if(closed||!validStoryThread()||
               lastSourceStage==null||episode<=0||
               lastSourceStage.originalEpisode!=episode||
               lastSourceStage.level!=level||lastSourceStage.stage!=stage||
               level<0||level>7||stage<0||stage>15||
               System.nanoTime()-lastSourceStageAtNs>3000000000L)return 0;
            try{
                if(phase==Phase.HOSTING&&host!=null&&host.isRunning()){
                    RoomStoryWire.Ticket ticket=host.storyTicket();
                    if(ticket==null||ticket.mission!=level||ticket.stage!=stage||
                       (originalRunningReadyTicket!=null&&
                        originalRunningReadyTicket.same(ticket)))return 0;
                    RoomWire.State state=host.snapshot();
                    if(state.revision!=ticket.rosterRevision||
                       state.players.size()!=Integer.bitCount(ticket.participants))
                        return 0;
                    // The return boolean is COMMIT, not whether the host
                    // source-scene-ready bit was installed successfully.
                    host.hostSceneLoaded(level,stage,ticket.sceneEpoch);
                    if(!host.hostLocalStageReady(ticket))return 0;
                    originalRunningReadyTicket=ticket;
                    event("ORIGINAL_HOST_LOCAL_RUNNING_STAGE_READY_NO_SHARED_AI");
                    return 1;
                }
                if(phase==Phase.JOINED&&client!=null&&client.isRunning()){
                    RoomStoryWire.Ticket ticket=client.storyTicket();
                    RoomWire.State state=client.snapshot();
                    if(ticket==null||state==null||client.storyCommitted()||
                       sceneEpoch<=0||ticket.sceneEpoch!=sceneEpoch||
                       ticket.mission!=level||ticket.stage!=stage||
                       ticket.rosterRevision!=(rosterRevision&0xffffffffL)||
                       ticket.token!=(((long)tokenHi<<32)|(tokenLo&0xffffffffL))||
                       state.revision!=ticket.rosterRevision||
                       !java.security.MessageDigest.isEqual(
                           state.session,ticket.session())||
                       state.players.size()!=Integer.bitCount(ticket.participants)||
                       guestSlot<2||guestSlot>4||
                       RoomWire.storySlot(state,state.self)!=guestSlot||
                       !ticket.includes(guestSlot)||
                       (originalRunningReadyTicket!=null&&
                        originalRunningReadyTicket.same(ticket)))return 0;
                    if(!client.acknowledgeSceneLoaded(level,stage,sceneEpoch))
                        return 0;
                    originalRunningReadyTicket=ticket;
                    event("ORIGINAL_GUEST_LOCAL_RUNNING_STAGE_READY_NO_SHARED_AI");
                    return 2;
                }
            }catch(IOException unavailable){return 0;}
            return 0;
        }
    }
    /** Source-only evidence for future launcher verification, not READY. */
    public OriginalStageCandidate originalStageCandidateFromGameThread(){
        synchronized(lock){
            if(!validStoryThread()||lastSourceStage==null||
               System.nanoTime()-lastSourceStageAtNs>60000000000L)return null;
            OriginalStageCandidate value=lastSourceStage;
            return new OriginalStageCandidate(value.originalEpisode,value.level,value.stage);
        }
    }
    private int lastStageResourceFolder=-1;
    private long lastStageResourceObservedAt;
    /** Two exact original Xbox level file OPENs + original game-thread player
     * sample. Does not prove selected mission, stage loaded or renderer. */
    boolean observeStageResourceFolderFromOriginalGameThread(int folder){
        synchronized(lock){
            if(closed||!validStoryThread()||
               (phase!=Phase.HOSTING&&phase!=Phase.JOINED)||
               (folder!=0&&folder!=1&&(folder<3||folder>8)))return false;
            // Native episode gate guarantees one callback per valid launch.
            // A new launch may reopen the SAME folder, so report again.
            lastStageResourceFolder=folder;
            lastStageResourceObservedAt=System.nanoTime();
            return true;
        }
    }
    /** Diagnostic, never used to trigger READY/COMMIT. */
    public int originalStageFolderHintFromGameThread(){
        synchronized(lock){
            if(!validStoryThread()||lastStageResourceFolder<0||
               System.nanoTime()-lastStageResourceObservedAt>60000000000L)return -1;
            return lastStageResourceFolder;
        }
    }
    boolean observeOriginalEyeFromGameThread(int x,int y,int z,int bx,int by,int bz){
        synchronized(lock){
            if(closed||(phase!=Phase.HOSTING&&phase!=Phase.JOINED)||
               x< -20000000||x>20000000||y< -20000000||y>20000000||
               z< -20000000||z>20000000||
               bx< -20000000||bx>20000000||by< -20000000||by>20000000||
               bz< -20000000||bz>20000000)return false;
            // This original player's native update is an actual game-thread
            // entry point, but does NOT prove a mission scene has loaded.
            if(storyGameThread==null)storyGameThread=Thread.currentThread();
            if(!validStoryThread())return false;
            lastEye=new LocalEye(x,y,z,System.nanoTime());
            // This is a genuine source-validated original PLAYER CAMERA sample,
            // not a confirmed body transform, remote game actor or stage proof.
            // Story still requires a separately verified engine scene-loaded
            // callback before COMMIT. Never initiate PREPARE/READY here.
            try{
                if(phase==Phase.JOINED&&client!=null&&client.storyCommitted()&&
                   localEyeTransportSeq<Integer.MAX_VALUE){
                    client.offerOriginalCameraEye(++localEyeTransportSeq,x,y,z,bx,by,bz);
                }else if(phase==Phase.HOSTING&&host!=null&&host.storyCommitted()&&
                         hostEyeWorldTick<Integer.MAX_VALUE){
                    java.util.List<RoomWorldSnapshotWire.Avatar> avatars=
                        new java.util.ArrayList<>(4);
                    // The real original actor's +0xA0 BODY FEET, not eye +0x100.
                    avatars.add(new RoomWorldSnapshotWire.Avatar(1,bx,by,bz,0,0));
                    // Never trust guest-reported body coordinates as a HOST
                    // world transform. Only bounded commands from an actual
                    // authenticated TLS peer may move a host-owned placeholder.
                    RoomStoryWire.Ticket exact=host.storyTicket();
                    if(exact!=null&&hostHumanPoses.observeHostSourceFeet(
                           exact,bx,by,bz))
                        avatars.addAll(hostHumanPoses.remotePoses(exact));
                    avatars.sort((a,b)->Integer.compare(a.playerId,b.playerId));
                    host.offerWorldSnapshot(++hostEyeWorldTick,avatars);
                }
            }catch(IOException sourceOrTransportError){
                // No identity, world coords or private error content in logs.
                // A single coarse code is sufficient for a later big trial.
                if(!eyeBridgeErrorLogged){
                    eyeBridgeErrorLogged=true;
                    event("ORIGINAL_CAMERA_AND_BODY_WORLD_TRANSPORT_REJECTED");
                }
            }
            return true;
        }
    }
    /** Only the bound original game thread may see recent camera samples. */
    public LocalEye localCameraEyeFromGameThread(){
        synchronized(lock){
            return validStoryThread()&&lastEye!=null&&
                System.nanoTime()-lastEye.atNs<=350000000L?lastEye:null;
        }
    }
    private long generation;
    private boolean closed;

    /** Read-only ticket status; no live socket or game actor is exposed. */
    public static final class StoryStatus {
        public final RoomStoryWire.Ticket ticket;
        public final boolean committed;
        private StoryStatus(RoomStoryWire.Ticket ticket,boolean committed){
            this.ticket=ticket;this.committed=committed;
        }
    }

    /**
     * Called exclusively from a future verified BLACK game-thread callback,
     * never from an Android button or a TLS reader. A new room must rebind.
     * This is a thread-identity guard, NOT a scene-load attestation.
     */
    public boolean bindStoryGameThread(){
        synchronized(lock){
            if(closed||(phase!=Phase.HOSTING&&phase!=Phase.JOINED))return false;
            if(storyGameThread==null)storyGameThread=Thread.currentThread();
            return storyGameThread==Thread.currentThread();
        }
    }
    private boolean validStoryThread(){
        return !closed&&storyGameThread!=null&&storyGameThread==Thread.currentThread();
    }
    /** Never auto-starts a level or publishes READY when a guest joins. */
    public StoryStatus storyStatus(){
        synchronized(lock){
            if(host!=null&&phase==Phase.HOSTING)
                return new StoryStatus(host.storyTicket(),host.storyCommitted());
            if(client!=null&&phase==Phase.JOINED&&client.isRunning())
                return new StoryStatus(client.storyTicket(),client.storyCommitted());
            return new StoryStatus(null,false);
        }
    }
    /**
     * Mission ID 0..7 from an independently verified game-thread selection
     * only. No UI or network-thread direct access; host is sole authority.
     */
    public RoomStoryWire.Ticket prepareStoryFromGameThread(int mission,int stage,int epoch)
        throws IOException {
        synchronized(lock){
            if(!validStoryThread()||phase!=Phase.HOSTING||host==null)
                throw new IOException("No bound host game thread");
            return host.prepareStory(mission,stage,epoch);
        }
    }
    /**
     * The caller MUST independently prove the actual BLACK scene is loaded.
     * Host return means all participants committed; guest return means READY
     * was QUEUED for its separate TLS network writer (NOT received by host).
     * Neither result, alone, proves real BLACK scene load; only an independently
     * verified game-thread loader callback may call this adapter.
     */
    public boolean reportSceneLoadedFromGameThread(int mission,int stage,int epoch)
        throws IOException {
        synchronized(lock){
            if(!validStoryThread())return false;
            if(host!=null&&phase==Phase.HOSTING)
                return host.hostSceneLoaded(mission,stage,epoch);
            if(client!=null&&phase==Phase.JOINED&&client.isRunning())
                return client.acknowledgeSceneLoaded(mission,stage,epoch);
            return false;
        }
    }
    /** Real TLS movement command, offered only from the verified BLACK game thread. */
    public boolean offerHumanInputFromGameThread(int seq,int x,int y,int yaw,
                                                  boolean fire,boolean jump)
        throws IOException{
        synchronized(lock){
            return validStoryThread()&&phase==Phase.JOINED&&client!=null&&
                client.offerHumanInput(seq,x,y,yaw,fire,jump);
        }
    }
    /** Trust comes from TLS peer identity; no direct NPC or guest memory writes. */
    public java.util.List<RoomTls.Server.TrustedInput> drainHumanInputsFromGameThread(){
        synchronized(lock){
            if(!validStoryThread()||phase!=Phase.HOSTING||host==null)
                return java.util.Collections.emptyList();
            java.util.List<RoomTls.Server.TrustedInput> accepted=
                host.drainLatestTrustedInputs();
            if(host.storyCommitted()){
                RoomStoryWire.Ticket exact=host.storyTicket();
                if(exact!=null){
                    final long now=System.nanoTime();
                    for(RoomTls.Server.TrustedInput sample:accepted)
                        hostHumanPoses.acceptAuthenticated(exact,sample.playerSlot,
                                                           sample.value,now);
                }
            }
            return accepted;
        }
    }
    /** Publish original HOST game-derived locations after verified COMMIT. */
    public boolean publishWorldSnapshotFromGameThread(int tick,
         java.util.List<RoomWorldSnapshotWire.Avatar> avatars)throws IOException{
        synchronized(lock){
            return validStoryThread()&&phase==Phase.HOSTING&&host!=null&&
                host.offerWorldSnapshot(tick,avatars);
        }
    }
    /** Original guest game thread reads latest authenticated HOST-only state. */
    public RoomWorldSnapshotWire.Snapshot readHostWorldFromGameThread(){
        synchronized(lock){
            if(!validStoryThread()||phase!=Phase.JOINED||client==null)
                return null;
            return client.worldSnapshot();
        }
    }
    /**
     * One authenticated TWO-HUMAN original body source for a future BOT LOOK.
     * Exact [epoch, rosterRevision, worldTick, localSlot, otherSlot,
     *        otherFeetXcm, otherFeetYcm, otherFeetZcm, yawCentiDeg, flags].
     *
     * Values are HOST-authoritative; the second phone never chooses an actor
     * pointer/transform. Source only, not an engine character or render call.
     * [0]*10 means no live currently committed exact 2-phone snapshot.
     */
    public int[] otherHumanBodySourceFromGameThread(){
        synchronized(lock){
            final int[] empty=new int[10];
            if(!validStoryThread())return empty;
            RoomStoryWire.Ticket t=phase==Phase.HOSTING&&host!=null?
                host.storyTicket():phase==Phase.JOINED&&client!=null?
                client.storyTicket():null;
            if(t==null||t.participants!=3||t.sceneEpoch<=0||
               t.rosterRevision<=0||t.rosterRevision>0xffffffffL)
                return empty;
            try{
                RoomWire.State state;
                int self,peer,tick;
                RoomWorldSnapshotWire.Avatar other=null;
                if(phase==Phase.HOSTING&&host!=null&&host.isRunning()&&
                   host.storyCommitted()){
                    state=host.snapshot();
                    self=RoomWire.storySlot(state,state.self);peer=2;
                    if(self!=1||hostEyeWorldTick<=0||lastEye==null||
                       System.nanoTime()-lastEye.atNs>350000000L)return empty;
                    for(RoomWorldSnapshotWire.Avatar v:
                        hostHumanPoses.remotePoses(t))
                        if(v.playerId==peer)other=v;
                    tick=hostEyeWorldTick;
                }else if(phase==Phase.JOINED&&client!=null&&
                         client.isRunning()&&client.storyCommitted()){
                    state=client.snapshot();
                    self=RoomWire.storySlot(state,state.self);peer=1;
                    if(self!=2)return empty;
                    RoomWorldSnapshotWire.Snapshot w=client.worldSnapshot();
                    if(w==null||!w.matches(t)||w.avatars.size()!=2)
                        return empty;
                    for(RoomWorldSnapshotWire.Avatar v:w.avatars)
                        if(v.playerId==peer)other=v;
                    tick=w.tick;
                }else return empty;
                if(state==null||other==null||tick<=0||
                   state.revision!=t.rosterRevision||
                   state.players.size()!=2||
                   !java.security.MessageDigest.isEqual(
                       state.session,t.session())||
                   !t.includes(self)||!t.includes(peer))
                    return empty;
                return new int[]{t.sceneEpoch,(int)t.rosterRevision,
                    tick,self,peer,other.xCm,other.yCm,other.zCm,
                    other.yawCentiDeg,other.flags};
            }catch(IOException invalid){return empty;}
        }
    }
    /** Host scene unload/cancel invalidates its prepared ticket. */
    public boolean cancelStoryFromGameThread(){
        synchronized(lock){
            return validStoryThread()&&phase==Phase.HOSTING&&host!=null&&
                   host.cancelStory();
        }
    }
    private Phase phase=Phase.IDLE;
    private String description="",fingerprint="";
    private RoomTls.Server host;
    private RoomDiscovery.Host announcer;
    private RoomTls.Client client;
    private RoomSession(){}
    public static RoomSession create(){return new RoomSession();}
    private static void closeQuietly(AutoCloseable resource){
        if(resource!=null)try{resource.close();}catch(Exception ignored){}
    }
    private long begin() {
        RoomTls.Server oldHost;
        RoomDiscovery.Host oldAnnouncer;
        RoomTls.Client oldClient;
        long token;
        synchronized(lock) {
            if(closed)throw new IllegalStateException("RoomSession was destroyed");
            token=++generation;
            storyGameThread=null;lastEye=null;
            hostHumanPoses.resetForNewRoom();
            localEyeTransportSeq=0;hostEyeWorldTick=0;eyeBridgeErrorLogged=false;
            lastStageResourceFolder=-1;lastStageResourceObservedAt=0;
            lastSourceStage=null;lastSourceStageAtNs=0;
            sourceStagePrepare.reset();
            originalGuestFrontendThread=null;
            originalRunningReadyTicket=null;
            oldHost=host;oldClient=client;oldAnnouncer=announcer;
            host=null;client=null;announcer=null;phase=Phase.CONNECTING;
            description="Connecting";fingerprint="";
        }
        closeQuietly(oldAnnouncer);closeQuietly(oldClient);closeQuietly(oldHost);
        event("SESSION_CONNECTING");
        return token;
    }
    private boolean current(long token) {
        synchronized(lock){return !closed && generation==token;}
    }
    private void failure(long token, Exception cause) {
        String message="Could not establish room";
        if(cause instanceof RoomTls.Rejected) {
            int reason=((RoomTls.Rejected)cause).reason;
            message=reason==2?"Incompatible game version":
                    reason==3?"Room is full":reason==5?"Too many failed attempts":"Room rejected";
        } else if(cause instanceof SSLHandshakeException) {
            message="Automatic connection failed; check that both phones use the same game build";
        } else if(cause instanceof GeneralSecurityException) {
            message="Could not prepare the automatic connection";
        } else if(cause instanceof SocketTimeoutException) {
            message="Network timeout: check Wi-Fi and access-point isolation";
        } else if(cause instanceof ConnectException) {
            message="Host unreachable: check Wi-Fi or VPN peer connectivity";
        } else if(cause instanceof UnknownHostException) {
            message="Invalid or unreachable host address";
        } else if(cause instanceof IOException) {
            message="Network connection closed or unavailable";
        }
        boolean published=false;
        synchronized(lock) {
            if(!closed && generation==token) {
                phase=Phase.ERROR;description=message;fingerprint="";published=true;
            }
        }
        if(published)event("ERROR_"+cause.getClass().getSimpleName()
            .replaceAll("[^A-Za-z0-9]","").toUpperCase(java.util.Locale.ROOT));
    }
    private void submit(Runnable action,long token){
        try{work.execute(action);}
        catch(java.util.concurrent.RejectedExecutionException e){failure(token,e);}
    }
    public void startHost(IdentitySource identity,InetAddress bind,int port,
                          String displayName,String roomName) {
        if(identity==null||bind==null)throw new IllegalArgumentException("Missing host settings");
        long token=begin();
        submit(()->{
            RoomTls.Server candidate=null;
            RoomDiscovery.Host broadcast=null;
            try {
                if(!current(token))return;
                // Port 0 is reserved for localhost-only isolated instrumentation.
                if(port>65535 || (port<1024 && !(port==0 && bind.isLoopbackAddress())))
                    throw new IOException("Invalid listener port");
                HostIdentity key=identity.load();
                if(!current(token))return;
                candidate=RoomTls.Server.start(key.tls,bind,port,displayName,roomName);
                candidate.setSafeEventSink(this::event);
                boolean discoverable=true;
                if(!bind.isLoopbackAddress()&&port!=0){
                    try{broadcast=RoomDiscovery.Host.start(candidate,bind,RoomDiscovery.PORT,key.fingerprint);}
                    catch(IOException unavailable){discoverable=false;}
                }
                boolean owned;
                synchronized(lock){
                    owned=!closed&&generation==token;
                    if(owned) {
                        host=candidate;announcer=broadcast;phase=Phase.HOSTING;
                        description=discoverable?"Hosting":"Hosting (LAN discovery unavailable)";
                        StringBuilder hex=new StringBuilder(64);
                        for(byte b:key.fingerprint)hex.append(String.format(java.util.Locale.ROOT,"%02X",b&255));
                        fingerprint=hex.toString();
                    }
                }
                if(!owned){closeQuietly(broadcast);closeQuietly(candidate);}
                else event(discoverable?"HOST_DISCOVERY_READY":"HOST_DISCOVERY_DISABLED");
            } catch(Exception e) {
                closeQuietly(broadcast);closeQuietly(candidate);
                failure(token,e);
            }
        },token);
    }
    public void join(String ipv4Port,String hostFingerprint,String displayName) {
        long token=begin();
        submit(()->{
            RoomTls.Client candidate=null;
            try {
                if(!current(token))return;
                if(hostFingerprint==null)candidate=RoomDirect.joinAutomatic(ipv4Port,displayName,()->!current(token));
                else {
                    RoomDirect.Endpoint endpoint=RoomDirect.parse(ipv4Port);
                    candidate=RoomTls.Client.connect(RoomTls.pinnedContext(RoomDirect.fingerprint(hostFingerprint)),endpoint.address,endpoint.port,displayName,()->!current(token));
                }
                // Always-on small safe codes, including after dismissing room UI.
                candidate.setSafeEventSink(this::event);
                boolean owned;
                synchronized(lock){
                    owned=!closed&&generation==token;
                    if(owned) {
                        client=candidate;phase=Phase.JOINED;description="Joined";
                    }
                }
                if(!owned)closeQuietly(candidate);
                else event("JOIN_CONNECTED");
            } catch(Exception e) {
                closeQuietly(candidate);
                failure(token,e);
            }
        },token);
    }
    public void joinAutomatic(String target,String name){join(target,null,name);}
    /** An immutable-by-copy view; no socket or peer pointer reaches UI/game code. */
    public Status snapshot(){
        synchronized(lock) {
            RoomWire.State roster=null;
            int port=0;
            if(host!=null) {
                port=host.port();
                try{roster=host.snapshot();}
                catch(IOException e){return new Status(Phase.ERROR,"Room unavailable","",0,null);}
            } else if(client!=null) {
                roster=client.snapshot();
                if(!client.isRunning()||roster==null)
                    return new Status(Phase.ERROR,"Disconnected","",0,null);
            }
            return new Status(phase,description,fingerprint,port,roster);
        }
    }
    public void stop(){
        RoomTls.Server oldHost;
        RoomDiscovery.Host oldAnnouncer;
        RoomTls.Client oldClient;
        synchronized(lock){
            ++generation;storyGameThread=null;lastEye=null;
            localEyeTransportSeq=0;hostEyeWorldTick=0;eyeBridgeErrorLogged=false;
            lastStageResourceFolder=-1;lastStageResourceObservedAt=0;
            lastSourceStage=null;lastSourceStageAtNs=0;
            sourceStagePrepare.reset();
            originalGuestFrontendThread=null;
            originalRunningReadyTicket=null;
            oldHost=host;oldClient=client;oldAnnouncer=announcer;
            host=null;client=null;announcer=null;phase=Phase.IDLE;description="";fingerprint="";
        }
        closeQuietly(oldAnnouncer);closeQuietly(oldClient);closeQuietly(oldHost);
        event("SESSION_STOPPED");
    }
    @Override public void close(){
        synchronized(lock){if(closed)return;closed=true;}
        stop();
        work.shutdownNow();
    }
}
