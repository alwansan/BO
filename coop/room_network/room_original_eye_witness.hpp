// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// READ ONLY! The original BLACK player-update hook already independently
// confirms the local original first-person player kind and identity.
// The source-confirmed eye at +0x100 is CAMERA EYE, NOT body root or stage ID.
// No network writing here, no guest memory writes, no scene READY signal.
#pragma once
#include "coop/room_original_eye_contract.hpp"
#include "coop/room_nonfrontend_original_player.hpp"
#include "coop/room_world_tick.hpp"
#include "kernel/coop_stage_resource_contract.hpp"
#include "coop/room_original_mission_field_hint.hpp"
#include "coop/room_original_streaming_stage_contract.hpp"
#include "coop/room_original_core_phase_witness.hpp"
#include "coop/room_original_core_stage_identity_contract.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
#include "coop/room_original_guest_launch_match_witness.hpp"
#include "coop/room_original_running_scene_source_gate.hpp"
#include "coop/room_other_human_body_source_witness.hpp"
#include "coop/room_original_live_remote_bot.hpp"
#include "game/layout.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <jni.h>
#include <atomic>
#include <cstdint>

namespace blackcoop {
inline void ObserveOriginalPlayerEye(uint32_t original_player)noexcept{
    if(!original_player)return;
    static std::atomic<uint64_t> lastMs{0};
    uint64_t at=static_cast<uint64_t>(SDL_GetTicks());
    uint64_t was=lastMs.load(std::memory_order_relaxed);
    if(was>0&&at>=was&&at-was<100)return; // at most 10Hz JNI samples
    if(!lastMs.compare_exchange_strong(was,at,std::memory_order_acq_rel))
        return;
    // Pinned RanggaCasper/Black-Android hooks.cpp: kPlayerEye=0x100,
    // read via G<float> only after current player object was validated.
    OriginalEyeCm result{};
    if(!QuantizeOriginalEye(kernel::G<float>(original_player+0x100),
                            kernel::G<float>(original_player+0x104),
                            kernel::G<float>(original_player+0x108),result))
        return;
    // The original actor BODY FEET are independently at +0xA0 (source-known
    // from the pinned MIT PC recomp's black_puppet_follow).
    OriginalBodyFeetCm body{};
    if(!QuantizeOriginalBodyFeet(kernel::G<float>(original_player+0xA0),
                                  kernel::G<float>(original_player+0xA4),
                                  kernel::G<float>(original_player+0xA8),body))return;
    // Same frontend state predicate as pinned original exit_menu.cpp.
    // A stable non-frontend original player is a diagnostic candidate only.
    // DO NOT send STORY_READY, PREPARE, COMMIT or modify original game state.
    uint32_t game=kernel::R32(layout::kGame);
    uint32_t world=kernel::R32(layout::kWorld);
    uint32_t state=game?kernel::R32(game+layout::kGameState):0;
    static NonFrontendOriginalPlayerWitness stable;
    if(stable.Observe(game,state,layout::kFrontEnd,world,true))
        SendSafeWorldCodeToAndroid(13); // coarse fixed event, NOT scene proof
    // A separate read-only source-linked ORIGINAL mission-core PHASE hint.
    // Pinned PC reference uses core+0x5AE0=0x1C to detect a running level;
    // owner original XBE CI44980186959 finds direct uses of +0x5AE0.
    // These facts do NOT yet verify game load completion or remote READY.
    const uint32_t coreEpisode=g_originalStageFileOpen.ActiveEpisode();
    const auto coreStream=g_originalStreamingStageHints.ForCorrelatedEpisode(coreEpisode);
    const bool paired=g_originalStageFileOpen.BothEssentialFilesOpenedForCandidate();
    static OriginalCoreRunningPhaseWitness corePhase;
    // A genuine original core pointer is still a HINT; guard all subsequent
    // source reads with pinned native Xbox guest-page commitment.
    const uint64_t phaseEnd=static_cast<uint64_t>(world)+0x5AE0u+4u;
    const bool phaseReadable=world&&phaseEnd<=0x100000000ull&&
        OriginalGuestSpanCommitted(world+0x5AE0u,4u);
    const uint32_t phase=phaseReadable?kernel::R32(world+0x5AE0u):0;
    const bool candidateReady=coreEpisode&&world&&phase==0x1Cu&&
        stable.IsStable()&&paired&&coreStream.episode==coreEpisode;
    static uint32_t phaseQualifiedEpisode=0,phaseQualifiedWorld=0;
    if(!candidateReady){
        phaseQualifiedEpisode=0;phaseQualifiedWorld=0;
    }
    if(corePhase.Observe(coreEpisode,world,phase,
                         stable.IsStable(),paired,coreStream.episode==coreEpisode)){
        phaseQualifiedEpisode=coreEpisode;phaseQualifiedWorld=world;
        SendSafeWorldCodeToAndroid(14); // source-gated RUNNING hint, NOT READY
    }
    // The MIT PC recomp checkpoint source reads canonical CORE level+stage
    // bytes at +0x5AEC/+0x5AED; unlike game+0x2020C/+0x2020E,
    // these may give a second independent stage-identity witness.
    // Guard the whole span on the genuine original game thread; this is
    // not proof of a loaded scene, safe loader ABI or host mission choice.
    const uint64_t identityEnd=static_cast<uint64_t>(world)+0x5AEDu+1u;
    const bool identityReadable=world&&identityEnd<=0x100000000ull&&
        OriginalGuestSpanCommitted(world+0x5AECu,2u);
    const int coreLevel=identityReadable?
        static_cast<int>(kernel::G<uint8_t>(world+0x5AECu)):-1;
    const int coreStage=identityReadable?
        static_cast<int>(kernel::G<uint8_t>(world+0x5AEDu)):-1;
    static OriginalCoreStageIdentityWitness coreStageIdentity;
    const int identityCode=coreStageIdentity.Observe(
        coreEpisode,world,candidateReady&&
        phaseQualifiedEpisode==coreEpisode&&phaseQualifiedWorld==world,
        coreStream.level,coreStream.stage,identityReadable,coreLevel,coreStage);
    if(identityCode)SendSafeWorldCodeToAndroid(identityCode); // 67..69 hints only
    // Additional P0-readiness proof: at most one safe coarse event per
    // readiness category / launch episode. NO model pointer ever exported,
    // NO original actor/character writes and NO scene/mission READY.
    ObserveOriginalBotPoolSourceHint(coreEpisode,candidateReady&&
        phaseQualifiedEpisode==coreEpisode&&phaseQualifiedWorld==world);
    // The source-match class is a stronger read-only identity than file
    // opens alone. No engine scene-ready callback is inferred from it.
    const bool lookSourceMatched=coreStageIdentity.Matched(coreEpisode,world);
    // Independent original launch+selected stage+original stable world
    // correlation. Diagnostic only: NO READY and NO guest loader call.
    const int guestLaunchMatch=GuestOriginalLaunchMatch().ObserveOriginalRunningWorld(
        coreEpisode,world,coreLevel,coreStage,lookSourceMatched);
    if(guestLaunchMatch)SendSafeWorldCodeToAndroid(guestLaunchMatch); // 81
    static OriginalRunningSceneSourceGate localSceneRun;
    const bool originalLocalSceneRunning=localSceneRun.Observe(
        coreEpisode,world,coreLevel,coreStage,lookSourceMatched);
    // Same genuine original game thread and independently stable loaded level.
    // The actual other-human BODY source is checked again at DRAW time.
    OriginalRemoteHumanBotVisual().ObserveScene(coreEpisode,world,
        originalLocalSceneRunning&&lookSourceMatched);
    JNIEnv* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    jobject activity=static_cast<jobject>(SDL_GetAndroidActivity());
    if(!env||!activity)return;
    jclass cls=env->GetObjectClass(activity);
    if(cls){
        jmethodID method=env->GetMethodID(cls,"coopRecordOriginalPlayerEye","(IIIIII)V");
        if(method)env->CallVoidMethod(activity,method,
            static_cast<jint>(result.x),static_cast<jint>(result.y),
            static_cast<jint>(result.z),static_cast<jint>(body.x),
            static_cast<jint>(body.y),static_cast<jint>(body.z));
        if(env->ExceptionCheck())env->ExceptionClear();
        // Only a source-matched ORIGINAL stage may become a value-only hint
        // for the room, never a scene-loaded report or STORY_READY.
        static uint64_t lastStageRefreshAt=0;
        if(!lookSourceMatched)lastStageRefreshAt=0;
        // Re-offer verified source stage at most every 2s, for late joiners.
        if(lookSourceMatched&&coreEpisode>0&&
           (identityCode==67||!lastStageRefreshAt||
            at<lastStageRefreshAt||at-lastStageRefreshAt>=2000u)){
            lastStageRefreshAt=at;
            jmethodID stageMethod=env->GetMethodID(
                cls,"coopRecordOriginalStageIdentity","(III)V");
            if(env->ExceptionCheck())env->ExceptionClear();
            else if(stageMethod)env->CallVoidMethod(activity,stageMethod,
                static_cast<jint>(coreEpisode),
                static_cast<jint>(coreLevel),static_cast<jint>(coreStage));
            if(env->ExceptionCheck())env->ExceptionClear();
        }
        // 2nd independent 30-sample original level-running gate. Host may
        // register its LOCAL stage readiness; guest must ALSO have the exact
        // source-matched original launch and same authenticated TLS ticket.
        // No game scene-loader, actor creation, NPC AI or guest memory writes.
        static uint64_t lastReadyAt=0;
        if(originalLocalSceneRunning&&
           (!lastReadyAt||at<lastReadyAt||at-lastReadyAt>=1000u)){
            lastReadyAt=at;
            OriginalGuestLaunchMatchKey guestKey{};
            const bool guestLaunch=GuestOriginalLaunchMatch().ReadMatchedKey(guestKey);
            if(!guestLaunch||guestKey.originalEpisode==coreEpisode){
                jmethodID readyMethod=env->GetMethodID(
                    cls,"coopAttestOriginalRunningScene","(IIIIIIII)V");
                if(env->ExceptionCheck())env->ExceptionClear();
                else if(readyMethod)env->CallVoidMethod(activity,readyMethod,
                    static_cast<jint>(coreEpisode),
                    static_cast<jint>(coreLevel),static_cast<jint>(coreStage),
                    static_cast<jint>(guestLaunch?guestKey.sceneEpoch:0),
                    static_cast<jint>(guestLaunch?guestKey.rosterRevision:0),
                    static_cast<jint>(guestLaunch?guestKey.tokenHi:0),
                    static_cast<jint>(guestLaunch?guestKey.tokenLo:0),
                    static_cast<jint>(guestLaunch?guestKey.selfSlot:0));
                if(env->ExceptionCheck())env->ExceptionClear();
            }
        }
        int peerMask=0;
        if(lookSourceMatched){
            jmethodID maskMethod=env->GetMethodID(
                cls,"coopHostAppearancePeerMaskFromGameThread","()I");
            if(env->ExceptionCheck())env->ExceptionClear();
            else if(maskMethod){
                peerMask=env->CallIntMethod(activity,maskMethod);
                if(env->ExceptionCheck()){
                    env->ExceptionClear();peerMask=0;
                }
            }
        }
        ObserveOriginalHumanLookPlanHint(coreEpisode,world,
            lookSourceMatched,static_cast<uint8_t>(peerMask));
        // Actually fetch the other REAL human's HOST-owned feet for a future
        // independent original-bot draw. Still NO original actor memory write.
        PollOriginalOtherHumanBodySource(env,activity,cls,
            originalLocalSceneRunning&&lookSourceMatched);
        env->DeleteLocalRef(cls);
    }else if(env->ExceptionCheck())env->ExceptionClear();
    env->DeleteLocalRef(activity);
    // Only a STABLE original non-frontend player and fresh file-open pair
    // from the SAME original launch episode. No stale previous mission files.
    // This STILL DOES NOT prove selected mission, scene loaded or safe READY.
    if(stable.IsStable()){
        int folder=-1;uint32_t episode=0;
        if(g_originalStageFileOpen.TakeCandidateFromOriginalPlayer(folder,episode)){
            SendSafeWorldCodeToAndroid(30+folder);
            // Pair three unverified original WORD XBE fields sampled only at
            // this launch with the actual later level resource file opens.
            // Exact candidate values 0..7 only, all other values one UNKNOWN
            // bucket. Nothing here calls a stage loader or STORY_READY.
            const auto hints=g_originalMissionFieldHints.ForCorrelatedEpisode(episode);
            if(hints.episode==episode){
                const int a=EncodeOriginalMissionFieldEvent(0,hints.readCandidate,folder);
                const int bPre=EncodeOriginalMissionFieldEvent(1,hints.launchBeforeCandidate,folder);
                const int bPost=EncodeOriginalMissionFieldEvent(2,hints.launchAfterCandidate,folder);
                const int c=EncodeOriginalMissionFieldEvent(3,hints.menuCandidate,folder);
                if(a>=0)SendSafeWorldCodeToAndroid(a);
                if(bPre>=0)SendSafeWorldCodeToAndroid(bPre);
                if(bPost>=0)SendSafeWorldCodeToAndroid(bPost);
                if(c>=0)SendSafeWorldCodeToAndroid(c);
            }
            // A stronger source-proven streaming ENTRY, correlated to the
            // SAME original launch episode and ORIGINAL successful level file
            // opens, but NEVER a loaded scene/mission ID/READY.
            const auto stream=g_originalStreamingStageHints.ForCorrelatedEpisode(episode);
            if(stream.episode==episode){
                const int event=EncodeOriginalStreamingStageHint(
                    stream.level,stream.stage,folder);
                if(event>=0)SendSafeWorldCodeToAndroid(event);
            }
        }
    }
}
} // namespace blackcoop
