"""Stage an opt-in *separate* BLACK title+TLS room APK without changing owner files.

This bridge does NOT create game actors, change saves, or make story co-op playable.
All anchors are checked in a disposable directory before writing any destination.
"""
import argparse
import hashlib
import json
import pathlib
import tempfile
from prepare_probe import BASE_FILES, GAME_FILES, once
from prepare_room_ui import prepare as prepare_room_ui

HERE = pathlib.Path(__file__).resolve().parent
PACKAGE = 'org.bocoop.black.roomnet'
INPUTS = BASE_FILES + GAME_FILES + ('src/game/exit_menu.cpp','src/game/difficulty.cpp','src/d3d/d3d8.cpp','native/input.cpp')


def prepare(root: pathlib.Path):
    root = root.resolve()
    originals = {name: (root / name).read_bytes() for name in INPUTS}
    with tempfile.TemporaryDirectory(prefix='bo-room-net-stage-') as directory:
        stage = pathlib.Path(directory)
        for name, data in originals.items():
            path = stage / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
        changed = prepare_room_ui(stage)
        pending = {name: (stage / name).read_text(encoding='utf-8') for name in changed}

    for name in pending:
        pending[name] = pending[name].replace('org.bocoop.black.roomui', PACKAGE)
        pending[name] = pending[name].replace('BLACK Co-op UI TEST', 'BLACK Co-op NETWORK TEST')

    # The read-only probe intentionally removed network access. Restore it only
    # to the separate, opt-in, authenticated room build; not the accepted UI APK.
    manifest = 'app/src/main/AndroidManifest.xml'
    text = pending[manifest]
    text = once(text, '    <application',
                '    <uses-permission android:name="android.permission.INTERNET" />\n'
                '    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />\n'
                '    <application', manifest)
    pending[manifest] = text

    java = 'app/src/main/java/com/racas/black/BlackActivity.java'
    text = pending[java]
    text = once(text, 'public class BlackActivity extends SDLActivity {',
                '''public class BlackActivity extends SDLActivity {
    // Invoked on the game UI thread through a narrow, no-guest-write JNI bridge.
    // HOST/JOIN are merely input forms; actual humans appear in a TLS roster.
    public void openCoopRoom(int row) {
        if (row != -102 && row != -103) return;
        runOnUiThread(() -> {
            if (isFinishing() || isDestroyed()) return;
            // Keeping SDL Activity foreground avoids the old pause/quit path.
            com.racas.black.coop.RoomOverlay.show(this,row);
        });
    }
    public String coopRoomStatusLine(int row) {
        return com.racas.black.coop.RoomAppRooms.statusLine(row);
    }
    // Called by genuine original local player update, NOT by Android UI,
    // NOT proof of loaded scene; safe, bounded camera eye coordinates only.
    public void coopRecordOriginalPlayerEye(int xCm,int yCm,int zCm,
                                            int bx,int by,int bz) {
        com.racas.black.coop.RoomAppRooms.originalPlayerEyeFromGameThread(xCm,yCm,zCm,bx,by,bz);
    }
    // Bounded <=20-int read-only ingress: actual TLS authenticated room peer
    // commands are consumed on the genuine BLACK game thread, NEVER UI.
    public int[] coopPollTrustedHumanInputsFromGameThread() {
        return com.racas.black.coop.RoomAppRooms.pollTrustedHumanInputsFromGameThread();
    }
    // Guest reads the current TLS-authenticated HOST canonical world on the
    // actual original BLACK game thread, never from a UI/network callback.
    public int[] coopPollHostWorldSnapshotFromGameThread() {
        return com.racas.black.coop.RoomAppRooms.pollHostWorldSnapshotFromGameThread();
    }
    // Obtain HOST-owned authenticated room roster ordinal slots only.
    // No guest coordinates, no actor pointers and no game scene READY.
    public int coopHostAppearancePeerMaskFromGameThread() {
        return com.racas.black.coop.RoomAppRooms.hostAppearancePeerMaskFromGameThread();
    }
    // Pinned original BLACK FRONTEND tick polls the verified host's
    // PREPARE without any local player body. Bounded 8 ints; no loader.
    public int[] coopPollGuestPreparedStoryFromOriginalFrontend() {
        return com.racas.black.coop.RoomAppRooms.guestPreparedStoryFromOriginalFrontend();
    }
    // Matched original core/stream level and stage as values on the genuine
    // BLACK game thread. NOT a loaded-scene callback, never auto PREPARE.
    public void coopRecordOriginalStageIdentity(int episode,int level,int stage) {
        com.racas.black.coop.RoomAppRooms.originalStageIdentityFromGameThread(
            episode,level,stage);
    }
    // Only the pinned original Xbox SDL gamepad report from the ORIGINAL
    // player callback is eligible; Java sends only after story COMMIT.
    public boolean coopOfferOriginalGuestController(int x,int y,int yaw,
                                                      boolean fire,boolean jump) {
        return com.racas.black.coop.RoomAppRooms.offerOriginalGuestControllerFromGameThread(
            x,y,yaw,fire,jump);
    }
''', java)
    text = once(text, '    @Override\n    protected void onPause() {',
                '''    @Override
    protected void onDestroy() {
        com.racas.black.coop.RoomAppRooms.shutdown();
        super.onDestroy();
    }

    @Override
    protected void onPause() {''', java)
    text = once(text, '        super.onCreate(savedInstanceState);',
                '        super.onCreate(savedInstanceState);\n        com.racas.black.coop.RoomAppRooms.attach(this);', java)
    pending[java] = text
    # Only an original local BLACK player-update hook may signal these
    # candidate transitions. This NEVER confirms a mission/scene or calls
    # RoomSession.reportSceneLoadedFromGameThread.
    text = pending[java]
    text = once(text, '    public String coopRoomStatusLine(int row) {',
        '''    public void coopRecordWorldTick(int code) {
        if (code == 1) com.racas.black.coop.RoomDiagnostics.event("WORLD_LOCAL_PLAYER_UPDATE_SEEN");
        else if (code == 2) com.racas.black.coop.RoomDiagnostics.event("WORLD_POINTER_CHANGED_UNVERIFIED");
        else if (code == 3) com.racas.black.coop.RoomDiagnostics.event("WORLD_MISSION_LAUNCH_ATTEMPT");
        else if (code == 4) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_DECL_CANDIDATE");
        else if (code == 5) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_SLOT12_PACKED_DECL");
        else if (code == 6) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_SLOT16_PACKED_DECL");
        else if (code == 7) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_SLOT20_PACKED_DECL");
        else if (code == 8) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_SLOT24_NORMPACK_DECL");
        else if (code == 9) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_SLOT28_FLOAT2_DECL");
        else if (code == 10) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_BOUND_INDEXED_DRAW");
        else if (code == 11) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_BOUND_ARRAY_DRAW");
        else if (code == 12) com.racas.black.coop.RoomDiagnostics.event("GPU_V36_BOUND_SAME_STREAM_12_20");
        else if (code == 13) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_NON_FRONTEND_PLAYER_STABLE_UNVERIFIED_MISSION");
        else if (code == 14) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_CORE_PHASE_1C_RESOURCE_STREAM_STABLE_SOURCE_HINT_NOT_READY");
        else if (code == 15) com.racas.black.coop.RoomDiagnostics.event("FIRST_AUTHENTICATED_HUMAN_INPUT_ON_ORIGINAL_GAME_THREAD_NO_ACTOR");
        else if (code == 16) com.racas.black.coop.RoomDiagnostics.event("FIRST_AUTHENTICATED_HOST_WORLD_ON_ORIGINAL_GUEST_GAME_THREAD_NO_RENDERER");
        else if (code == 60) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_POOL_NO_ELIGIBLE_BOT_LOOK_SOURCE_NOT_READY");
        else if (code == 61) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_POOL_BOT_LOOK_BUT_ZERO_BLANK_RIG_SLOTS_NOT_READY");
        else if (code == 62) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_POOL_BOT_LOOK_ONE_BLANK_RIG_SLOT_NOT_READY");
        else if (code == 63) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_POOL_BOT_LOOK_TWO_BLANK_RIG_SLOTS_NOT_READY");
        else if (code == 64) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_POOL_BOT_LOOK_THREE_OR_MORE_BLANK_RIG_SLOTS_NOT_READY");
        else if (code == 65) com.racas.black.coop.RoomDiagnostics.event("FIRST_SOURCE_ORIGINAL_GUEST_MERGED_INPUT_QUEUED_NOT_GAMEPLAY");
        else if (code == 66) com.racas.black.coop.RoomDiagnostics.event("FIRST_ORIGINAL_GUEST_TOUCH_INPUT_QUEUED_NOT_GAMEPLAY");
        else if (code == 67) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_CORE_LEVEL_STAGE_MATCH_STREAM_SOURCE_HINT_NOT_READY");
        else if (code == 68) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_CORE_LEVEL_STAGE_CONFLICT_STREAM_SOURCE_HINT_NOT_READY");
        else if (code == 69) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_CORE_LEVEL_STAGE_UNKNOWN_SOURCE_HINT_NOT_READY");
        else if (code == 70) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_HUMAN_LOOK_ONE_RIG_PROPOSED_NO_ACTOR");
        else if (code == 71) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_HUMAN_LOOK_TWO_RIGS_PROPOSED_NO_ACTOR");
        else if (code == 72) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_HUMAN_LOOK_THREE_RIGS_PROPOSED_NO_ACTOR");
        else if (code == 73) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_HUMAN_LOOK_NO_ORIGINAL_BOT_TEMPLATE");
        else if (code == 74) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_HUMAN_LOOK_NOT_ENOUGH_SAFE_BLANK_SLOTS");
        else if (code == 75) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_HUMAN_LOOK_PROPOSAL_INVALIDATED_NO_ACTOR");
        else if (code == 76) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_GUEST_HOST_STAGE_ON_FRONTEND_ENGINE_THREAD_NO_LOADER");
        else if (code == 77) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_GUEST_HOST_STAGE_CHANGED_ON_FRONTEND_THREAD_NO_LOADER");
        else if (code == 78) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_GUEST_SELECTED_LEVEL_MATCHES_AUTH_HOST_STAGE_NO_READY");
        else if (code == 79) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_GUEST_SELECTED_LEVEL_CONFLICTS_AUTH_HOST_STAGE_NO_READY");
        else if (code == 81) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_GUEST_RUNNING_LEVEL_MATCHES_HOST_STAGE_NO_SHARED_WORLD");
        else if (code == 20) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_LAUNCH_DIFFICULTY_EASY_NOT_MISSION");
        else if (code == 21) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_LAUNCH_DIFFICULTY_NORMAL_NOT_MISSION");
        else if (code == 22) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_LAUNCH_DIFFICULTY_HARD_NOT_MISSION");
        else if (code == 23) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_LAUNCH_DIFFICULTY_BLACK_OPS_NOT_MISSION");
        else if (code == 24) com.racas.black.coop.RoomDiagnostics.event("ORIGINAL_LAUNCH_DIFFICULTY_UNAVAILABLE");
        else if (code >= 30 && code <= 38 && code != 32)
            com.racas.black.coop.RoomAppRooms.originalLevelResourceHintFromGameThread(code - 30);
        else if (code >= 100 && code <= 423)
            com.racas.black.coop.RoomAppRooms.originalMissionFieldCandidateFromGameThread(code);
        else if (code >= 500 && code <= 2029)
            com.racas.black.coop.RoomAppRooms.originalStreamingStageFromGameThread(code);
    }
    public String coopRoomStatusLine(int row) {''', java)
    pending[java] = text

    # Host/Join in the accepted Flash page dispatch to a same-Activity dialog, which
    # owns TLS; no guest actor is created and no network worker touches Flash.
    menu = 'src/game/options_menu.cpp'
    text = pending[menu]
    text = once(text, '#include "coop/room_ui.h"',
                '#include "coop/room_ui.h"\n#include "coop/room_net_ui.hpp"', menu)
    values = {
        '-101': 'OFFLINE / PREVIEW',
        '-102': 'NOT AVAILABLE',
        '-103': 'NOT AVAILABLE',
        '-104': '2-4 PLANNED',
        '-105': 'LAN / VPN PLANNED',
    }
    for ident,old in values.items():
        text = once(text, f'case {ident}: return "{old}";',
                    f'case {ident}: return blackcoop::AndroidRoomRow({ident});', menu)
    # The accepted read-only UI deliberately intercepted all synthetic ROOM
    # rows in UpdateRow. Upgrade only HOST and JOIN to explicit confirmed
    # actions; preserve scrolling and never forward negative option IDs.
    needle = '''    if (IsRoomRow(row)) {
        // Still allow moving through all the read-only rows with the D-pad.
        // This handler must never forward these placeholder IDs to options.
        if (!g_page.extra && UpdateScroll(ctx, slot, id))
            W32(ui + kMenuInput, 0);
        return;
    }'''
    replacement = '''    if (IsRoomRow(row)) {
        if (!g_page.extra && g_page.mode == kEntries &&
            (row->id == -102 || row->id == -103) && id == kInputConfirm) {
            blackcoop::OpenAndroidLobby(row->id);
            W32(ui + kMenuInput, 0);
        } else if (!g_page.extra && UpdateScroll(ctx, slot, id)) {
            W32(ui + kMenuInput, 0);
        }
        return;
    }'''
    text = once(text, needle, replacement, menu)
    pending[menu] = text

    # Compile Java sources in the game APK's existing com.racas.black namespace.
    java_sources = {
        'RoomWire.java': 'lobby/java/com/racas/black/coop/RoomWire.java',
        'RoomStoryWire.java': 'lobby/java/com/racas/black/coop/RoomStoryWire.java',
        'RoomHumanInputWire.java': 'lobby/java/com/racas/black/coop/RoomHumanInputWire.java',
        'RoomWorldSnapshotWire.java': 'lobby/java/com/racas/black/coop/RoomWorldSnapshotWire.java',
        'RoomCameraEyeWire.java': 'lobby/java/com/racas/black/coop/RoomCameraEyeWire.java',
        'RoomTls.java': 'lobby/java/com/racas/black/coop/RoomTls.java',
        'RoomDirect.java': 'lobby/java/com/racas/black/coop/RoomDirect.java',
        'RoomDiscovery.java': 'lobby/java/com/racas/black/coop/RoomDiscovery.java',
        'RoomSession.java': 'lobby/java/com/racas/black/coop/RoomSession.java',
        'RoomOriginalHostStagePrepareGate.java': 'lobby/java/com/racas/black/coop/RoomOriginalHostStagePrepareGate.java',
        'RoomHostHumanKinematics.java': 'lobby/java/com/racas/black/coop/RoomHostHumanKinematics.java',
        'RoomGuestHostWorldSource.java': 'lobby/java/com/racas/black/coop/RoomGuestHostWorldSource.java',
        'RoomScanGate.java': 'lobby/java/com/racas/black/coop/RoomScanGate.java',
        'RoomPeerTrust.java': 'lobby/java/com/racas/black/coop/RoomPeerTrust.java',
        'RoomPresence.java': 'lobby/java/com/racas/black/coop/RoomPresence.java',
        'RoomIdentity.java': 'lobby/android/com/racas/black/coop/RoomIdentity.java',
        'RoomAppRooms.java': 'lobby/android/com/racas/black/coop/RoomAppRooms.java',
        'RoomOverlay.java': 'lobby/android/com/racas/black/coop/RoomOverlay.java',
        'RoomDiagnostics.java': 'lobby/android/com/racas/black/coop/RoomDiagnostics.java',
    }
    for name, source in java_sources.items():
        destination = 'app/src/main/java/com/racas/black/coop/' + name
        content = (HERE / source).read_text(encoding='utf-8')
        pending[destination] = content

    pending['src/coop/room_net_ui.hpp'] = (HERE / 'room_network/room_net_ui.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_world_tick.hpp'] = (HERE / 'room_network/room_world_tick.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_eye_contract.hpp'] = (HERE / 'room_network/room_original_eye_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_nonfrontend_original_player.hpp'] = (HERE / 'room_network/room_nonfrontend_original_player.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_launch_difficulty_contract.hpp'] = (HERE / 'room_network/room_original_launch_difficulty_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_mission_field_hint.hpp'] = (HERE / 'room_network/room_original_mission_field_hint.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_launch_difficulty_witness.hpp'] = (HERE / 'room_network/room_original_launch_difficulty_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_eye_witness.hpp'] = (HERE / 'room_network/room_original_eye_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_streaming_stage_contract.hpp'] = (HERE / 'room_network/room_original_streaming_stage_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_streaming_stage_witness.hpp'] = (HERE / 'room_network/room_original_streaming_stage_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_core_phase_witness.hpp'] = (HERE / 'room_network/room_original_core_phase_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_core_stage_identity_contract.hpp'] = (HERE / 'room_network/room_original_core_stage_identity_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_bot_pool_contract.hpp'] = (HERE / 'room_network/room_original_bot_pool_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_bot_pool_witness.hpp'] = (HERE / 'room_network/room_original_bot_pool_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_local_pad_tap.hpp'] = (HERE / 'room_network/room_original_local_pad_tap.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_guest_pad_contract.hpp'] = (HERE / 'room_network/room_original_guest_pad_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_guest_pad_witness.hpp'] = (HERE / 'room_network/room_original_guest_pad_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_trusted_input_contract.hpp'] = (HERE / 'room_network/room_original_trusted_input_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_trusted_input_witness.hpp'] = (HERE / 'room_network/room_original_trusted_input_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_guest_host_world_contract.hpp'] = (HERE / 'room_network/room_original_guest_host_world_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_guest_host_world_witness.hpp'] = (HERE / 'room_network/room_original_guest_host_world_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_guest_prepared_story_contract.hpp'] = (HERE / 'room_network/room_original_guest_prepared_story_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_guest_prepared_story_witness.hpp'] = (HERE / 'room_network/room_original_guest_prepared_story_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_guest_launch_match_contract.hpp'] = (HERE / 'room_network/room_original_guest_launch_match_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/room_original_guest_launch_match_witness.hpp'] = (HERE / 'room_network/room_original_guest_launch_match_witness.hpp').read_text(encoding='utf-8')
    pending['src/coop/world_tick_contract.hpp'] = (HERE / 'room_network/world_tick_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/gpu_decl_contract.hpp'] = (HERE / 'room_network/gpu_decl_contract.hpp').read_text(encoding='utf-8')
    pending['src/coop/gpu_decl_witness.hpp'] = (HERE / 'room_network/gpu_decl_witness.hpp').read_text(encoding='utf-8')

    # HOOK001/video 2026-11-22 proves the owner drives the ORIGINAL game
    # through the native touchscreen. Original Android src/native/input.cpp
    # applies touch::Apply(port,p) to the REAL merged port-0 Xbox report.
    # Capture that report ONCE at its actual source without any second Poll(0):
    # a second call also invokes app::kGame.pad and can alter input semantics.
    pad_file='native/input.cpp'
    pad=originals[pad_file].decode('utf-8').replace('\r\n','\n')
    pad=once(pad,'#include "host.h"',
             '#include "host.h"\n#include "coop/room_original_local_pad_tap.hpp"',pad_file)
    pad=once(pad,'''    touch::Apply(port, p);
    if (app::kGame.pad) app::kGame.pad(port, p);''',
    '''    // Original local input delivered unchanged; one read-only snapshot
    // AFTER the upstream merges SDL, Android loose controller and TOUCH.
    // No second input::Poll(0), no gamepad callback duplication.
    const Pad beforeTouch=p;
    touch::Apply(port, p);
    if (port==0) {
        const bool addedTouch=beforeTouch.connected!=p.connected ||
            beforeTouch.buttons!=p.buttons ||
            beforeTouch.lx!=p.lx || beforeTouch.ly!=p.ly ||
            beforeTouch.analog[0]!=p.analog[0] ||
            beforeTouch.analog[7]!=p.analog[7];
        blackcoop::LocalPadTap().Capture({
            p.connected,addedTouch,p.lx,p.ly,p.analog[0],p.analog[7]},
            static_cast<uint64_t>(SDL_GetTicks()));
    }
    if (app::kGame.pad) app::kGame.pad(port, p);''',pad_file)
    pending[pad_file]=pad

    # Staging-only extension of the EXISTING original local-player X86 wrapper.
    # It samples R32(kWorld) only when the original character kind and identity
    # already match the original first-person player. No writes to guest RAM.
    cheat='src/game/cheats.cpp'
    game=pending.get(cheat,originals[cheat].decode('utf-8'))
    game=once(game,'#include "game/layout.h"',
              '#include "game/layout.h"\n#include "coop/room_world_tick.hpp"\n#include "coop/room_original_eye_witness.hpp"\n#include "coop/room_original_trusted_input_witness.hpp"',cheat)
    game=once(game,'    __imp__sub_001E5FA0(ctx, base);',
        '''    // Candidate only: first original-player update in this world is
    // NOT proof that host-selected mission/stage loaded on both phones.
    if (me && me == Player() && R32(me + kCharKind) == kPlayerKind) {
        blackcoop::ObserveLocalWorldTick(R32(kWorld));
        // Original BLACK eye vector known from pinned upstream hooks.cpp;
        // sample only, never a proxy actor or fabricated scene READY.
        blackcoop::ObserveOriginalPlayerEye(me);
        // Authenticated commands are polled at 40Hz on original game thread.
        // This does NOT dispatch commands to bots or create an actor.
        blackcoop::ObserveOriginalHostInputTick();
    }
    __imp__sub_001E5FA0(ctx, base);''',cheat)
    pending[cheat]=game

    # The pinned source already wraps this genuine difficulty-menu launch
    # candidate. We observe invocation only; it can return without loading.
    # Stage/mission IDs remain unknown, so NEVER publish a story PREPARE/READY.
    difficulty='src/game/difficulty.cpp'
    # Git for Windows commonly checks out the pinned upstream CPP as CRLF.
    # The following stage-source anchor contains an LF before the function's
    # closing brace; normalize ONLY this disposable staged source FIRST.
    # Original upstream files remain untouched and output stays LF.
    launch=pending.get(difficulty,originals[difficulty].decode('utf-8')).replace('\r\n','\n')
    launch=once(launch,'#include "xdk/hle.h"',
                '#include "xdk/hle.h"\n#include "coop/room_world_tick.hpp"\n#include "coop/room_original_launch_difficulty_witness.hpp"\n#include "coop/room_original_streaming_stage_witness.hpp"\n#include "coop/room_original_guest_prepared_story_witness.hpp"',difficulty)
    # Pinned upstream original FRONTEND frame wrapper. In PREPARE phase the
    # guest has no body, so original player-update hooks never run. Poll
    # only the verified TLS ticket on this actual original frontend tick.
    launch=once(launch,
        '    Unlock();\n    __imp__sub_00115030(ctx, base);\n    OfferBlackOps();',
        '    Unlock();\n    blackcoop::ObserveOriginalGuestPreparedStoryFrontend();\n    __imp__sub_00115030(ctx, base);\n    OfferBlackOps();',difficulty)
    launch=once(launch,'    __imp__sub_00124CF0(ctx, base);',
        '''    blackcoop::ObserveMissionLaunchCandidate(); // attempt, NOT loaded scene
    const uint32_t originalLaunchEpisode=blackcoop::ObserveOriginalLaunchDifficulty();
    __imp__sub_00124CF0(ctx, base); // ORIGINAL call unchanged
    blackcoop::ObserveOriginalLaunchReturn(originalLaunchEpisode); // B AFTER, WORD''',difficulty)
    # Pinned c5 XBE CI44979193377 confirms two BYTE loads inside this
    # streaming function. Observe them only; preserve its native work.
    launch=once(launch,
        '    blackcoop::ObserveOriginalLaunchReturn(originalLaunchEpisode); // B AFTER, WORD\n}',
        '''    blackcoop::ObserveOriginalLaunchReturn(originalLaunchEpisode); // B AFTER, WORD
}

// First ORIGINAL streaming entry only: candidate LEVEL/STAGE BYTE read,
// NEVER guest auto-launch or actual scene-loaded signal.
X86_EXTERN_FUNC(__imp__sub_001EDC90);
X86_FUNC(sub_001EDC90) {
    blackcoop::ObserveOriginalStreamingStageEntry();
    __imp__sub_001EDC90(ctx, base); // unchanged original game function
}''',difficulty)
    pending[difficulty]=launch

    # Read-only observation of the ORIGINAL title's actual parsed Xbox
    # D3D8 vertex declaration. This is not model skinning proof or a GPU draw.
    # Six fixed-code categories, each emitted ONCE; never record shader bytes,
    # shader registers/constants, guest addresses or vertex payloads.
    d3d='src/d3d/d3d8.cpp'
    shader=pending.get(d3d,originals[d3d].decode('utf-8'))
    shader=once(shader,'#include "gpu/vertex.h"',
                '#include "gpu/vertex.h"\n#include "coop/gpu_decl_witness.hpp"',d3d)
    shader=once(shader,'    gpu::ParseDeclaration(*vs);',
                 '    gpu::ParseDeclaration(*vs);\n    blackcoop::ObserveOriginalGpuDecl(*vs); // fixed event categories only',d3d)
    # Original D3D8 draw is a stronger witness than shader *creation*:
    # require a real bound vertex buffer and 36-byte declared/active strides.
    # These calls do NOT inspect the MODEL resource or alter the backend draw.
    shader=once(shader,
        '    if (g_backend) g_backend->Draw(g_state, a[0], a[1], a[2], nullptr);',
        '''    if (g_backend && a[2] != 0)
        blackcoop::ObserveOriginalGpuDraw(g_state, false); // bounded draw witness
    if (g_backend) g_backend->Draw(g_state, a[0], a[1], a[2], nullptr);''',d3d)
    shader=once(shader,
        '    if (g_backend) g_backend->Draw(g_state, a[0], 0, a[1], indices);',
        '''    if (g_backend && a[1] != 0)
        blackcoop::ObserveOriginalGpuDraw(g_state, true); // bounded indexed witness
    if (g_backend) g_backend->Draw(g_state, a[0], 0, a[1], indices);''',d3d)
    pending[d3d]=shader
    for name in pending:
        if name not in originals and (root / name).exists():
            raise ValueError('Refusing preexisting addition: ' + name)
        if name in originals and originals[name].decode('utf-8').replace('\r\n','\n') == pending[name]:
            raise ValueError('Staged patch did not change existing file: ' + name)
    # Do not publish any source/assets from the actual ISO. Stages are source-only.
    for name, content in pending.items():
        path = root / name
        path.parent.mkdir(parents=True,exist_ok=True)
        path.write_text(content,encoding='utf-8',newline='\n')
    return {name: hashlib.sha256((root / name).read_bytes()).hexdigest() for name in pending}


if __name__ == '__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root',type=pathlib.Path)
    args=parser.parse_args()
    print(json.dumps({'package':PACKAGE,'changed_files':prepare(args.root)},indent=2))
