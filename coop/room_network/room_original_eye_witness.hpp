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
    if(corePhase.Observe(coreEpisode,world,world?kernel::R32(world+0x5AE0u):0,
                         stable.IsStable(),paired,coreStream.episode==coreEpisode))
        SendSafeWorldCodeToAndroid(14); // source-gated RUNNING hint, NOT READY
    JNIEnv* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    jobject activity=static_cast<jobject>(SDL_GetAndroidActivity());
    if(!env||!activity)return;
    jclass cls=env->GetObjectClass(activity);
    if(cls){
        jmethodID method=env->GetMethodID(cls,"coopRecordOriginalPlayerEye","(III)V");
        if(method)env->CallVoidMethod(activity,method,
            static_cast<jint>(result.x),static_cast<jint>(result.y),
            static_cast<jint>(result.z));
        if(env->ExceptionCheck())env->ExceptionClear();
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
