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
INPUTS = BASE_FILES + GAME_FILES + ('src/game/exit_menu.cpp',)


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
        'RoomTls.java': 'lobby/java/com/racas/black/coop/RoomTls.java',
        'RoomDirect.java': 'lobby/java/com/racas/black/coop/RoomDirect.java',
        'RoomDiscovery.java': 'lobby/java/com/racas/black/coop/RoomDiscovery.java',
        'RoomSession.java': 'lobby/java/com/racas/black/coop/RoomSession.java',
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
    pending['src/coop/world_tick_contract.hpp'] = (HERE / 'room_network/world_tick_contract.hpp').read_text(encoding='utf-8')

    # Staging-only extension of the EXISTING original local-player X86 wrapper.
    # It samples R32(kWorld) only when the original character kind and identity
    # already match the original first-person player. No writes to guest RAM.
    cheat='src/game/cheats.cpp'
    game=pending.get(cheat,originals[cheat].decode('utf-8'))
    game=once(game,'#include "game/layout.h"',
              '#include "game/layout.h"\n#include "coop/room_world_tick.hpp"',cheat)
    game=once(game,'    __imp__sub_001E5FA0(ctx, base);',
        '''    // Candidate only: first original-player update in this world is
    // NOT proof that host-selected mission/stage loaded on both phones.
    if (me && me == Player() && R32(me + kCharKind) == kPlayerKind)
        blackcoop::ObserveLocalWorldTick(R32(kWorld));
    __imp__sub_001E5FA0(ctx, base);''',cheat)
    pending[cheat]=game
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
