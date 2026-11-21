"""Prepare a disposable read-only HOOK001 build from the pinned BLACK v1.5.0 source.

Preflight every exact source anchor before writing to the target checkout.
Never modify the original BO APK, successful baseline, original save or XBE.
Only source code from coop/probe is copied; licensed disc assets are build inputs
used in temporary runners, never published.
"""
import argparse
import hashlib
import json
import pathlib
import shutil
import tempfile

from prepare_baseline import prepare as prepare_baseline

PACKAGE = "org.bocoop.black.probe2"
BASE_FILES = (
    "app/build.gradle",
    "app/src/main/AndroidManifest.xml",
    "app/src/main/res/values/strings.xml",
    "app/src/main/java/com/racas/black/UpdateChecker.java",
    "app/src/main/java/com/racas/black/BlackActivity.java",
)
GAME_FILES = ("src/game/cheats.cpp", "src/game/hooks.cpp", "src/game/options_menu.cpp")
ANDROID_FILES = ("ProbeLogExporter.java",)
PROBE_FILES = ("probe_core.hpp", "probe.h", "probe.cpp")

def once(source, old, new, name):
    if source.count(old) != 1:
        raise ValueError(f"Expected exactly one unchanged anchor in {name}: {old[:70]!r}")
    return source.replace(old, new, 1)

def prepare(root: pathlib.Path, probe_dir: pathlib.Path):
    root = root.resolve()
    probe_dir = probe_dir.resolve()
    # Validation is deliberately before ANY write to the real upstream tree.
    original = {}
    for name in BASE_FILES + GAME_FILES:
        original[name] = (root/name).read_text(encoding="utf-8")
    source_files = {}
    for name in PROBE_FILES:
        source_files[f"src/coop/{name}"] = (probe_dir/name).read_text(encoding="utf-8")
    for name in ANDROID_FILES:
        source_files[f"app/src/main/java/com/racas/black/{name}"] = (
            probe_dir.parent/"android"/name).read_text(encoding="utf-8")
    if "MediaStore.Downloads.EXTERNAL_CONTENT_URI" not in source_files[
            "app/src/main/java/com/racas/black/ProbeLogExporter.java"]:
        raise ValueError("MediaStore Downloads export must be explicit")
    if "Java_com_racas_black_BlackActivity_nativeCoopProbeEnable" not in source_files["src/coop/probe.cpp"]:
        raise ValueError("The expected static native JNI entry is absent")
    if "#include <algorithm>" not in source_files["src/coop/probe.cpp"]:
        raise ValueError("Sampler must explicitly include std::copy's header")

    # Run the already device-accepted baseline's packaging transform on a
    # disposable TEXT fixture, to share its restrictive storage/import logic
    # without mutating the tested baseline implementation or its workflow.
    with tempfile.TemporaryDirectory(prefix="bo-probe-plan-") as d:
        stage = pathlib.Path(d)
        for name in BASE_FILES:
            path = stage/name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(original[name], encoding="utf-8")
        prepare_baseline(stage)
        pending = {name:(stage/name).read_text(encoding="utf-8") for name in BASE_FILES}

    for name in (
        "app/build.gradle",
        "app/src/main/java/com/racas/black/UpdateChecker.java",
        "app/src/main/java/com/racas/black/BlackActivity.java",
    ):
        pending[name] = pending[name].replace("org.bocoop.black.baseline", PACKAGE)
        if "org.bocoop.black.baseline" in pending[name] or PACKAGE not in pending[name]:
            raise ValueError(f"Did not fully isolate new probe package in {name}")

    # Capturing is completely local; unlike the original game and baseline,
    # the separate probe has no INTERNET or NETWORK_STATE permission.
    manifest = "app/src/main/AndroidManifest.xml"
    pending[manifest] = once(pending[manifest],
        '<uses-permission android:name="android.permission.INTERNET" />', "", manifest)
    pending[manifest] = once(pending[manifest],
        '<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />', "", manifest)

    label = "app/src/main/res/values/strings.xml"
    pending[label] = once(pending[label], "BLACK Baseline - NO CO-OP",
                          "BLACK Probe 2 - READ ONLY", label)

    java = "app/src/main/java/com/racas/black/BlackActivity.java"
    text = pending[java]
    text = once(text, 'setTitle("BLACK Baseline")', 'setTitle("BLACK Probe 2")', java)
    text = once(text,
        "    private boolean checking;   // a check asked from the menu is under way",
        "    private boolean checking;   // a check asked from the menu is under way\n"
        "    private boolean coopProbePrompted;\n"
        "    private final ProbeLogExporter probeLogExporter = new ProbeLogExporter(this);\n",java)
    text = once(text, "    private static native void nativeLooseRelease();",
        "    private static native void nativeLooseRelease();\n"
        "    private static native void nativeCoopProbeEnable();", java)
    text = once(text,
        "        askForGame();\n        tellOfUpdate();\n    }\n",
        "        askForGame();\n        tellOfUpdate();\n        offerCoopProbe();\n    }\n", java)
    text = once(text,
        "            if (!ok) askForGame();\n            tellOfUpdate();",
        "            if (!ok) askForGame();\n            tellOfUpdate();\n"
        "            if (ok) offerCoopProbe();", java)
    java_method = r'''    // HOOK001 is off unless the user explicitly presses Start. Once per launch.
    // No broad storage access, no network requests, no gameplay modification.
    private void offerCoopProbe() {
        if (coopProbePrompted || importing || isFinishing() || mBrokenLibraries
                || (dialog != null && dialog.isShowing())) return;
        File own = getExternalFilesDir(null);
        if (own == null || !hasGame(new File(own, "assets"))) return;
        coopProbePrompted = true;
        dialog = new AlertDialog.Builder(this)
                .setTitle("BLACK Probe 2 - READ ONLY")
                .setMessage("Optional diagnostic capture of local character observations.\n\n"
                        + "If you start capture, readable .txt snapshots of the probe and game logs "
                        + "are exported to Download/Games/cl/black log/logs/BLACK-Probe-[date]/. "
                        + "These shared files may be read by other apps; do not post private logs publicly. "
                        + "No network, actor creation or save changes. "
                        + "Capture ends automatically (up to 180 seconds in the world).")
                .setPositiveButton("Start capture", (d, w) -> {
                    try {
                        nativeCoopProbeEnable();
                        probeLogExporter.start();
                        Toast.makeText(this, "Read-only capture enabled for this launch",
                                Toast.LENGTH_LONG).show();
                    } catch (UnsatisfiedLinkError err) {
                        Toast.makeText(this, "Diagnostic unavailable", Toast.LENGTH_LONG).show();
                    }
                })
                .setNegativeButton("Play without capture", (d, w) -> {})
                .show();
    }

'''
    text = once(text, "    private void startImport() {", java_method + "    private void startImport() {", java)
    text = once(text, "    protected void onPause() {\n        resumed = false;",
                "    protected void onPause() {\n        probeLogExporter.snapshot();\n        resumed = false;", java)
    pending[java] = text

    cheats = "src/game/cheats.cpp"
    text = original[cheats]
    text = once(text,'#include "game/layout.h"', '#include "game/layout.h"\n#include "coop/probe.h"', cheats)
    text = once(text,'X86_FUNC(sub_001E5FA0) {\n    uint32_t me = ctx.ecx;',
                    'X86_FUNC(sub_001E5FA0) {\n    uint32_t me = ctx.ecx;\n'
                    '    bo_probe::ObservePlayer(me,base);', cheats)
    pending[cheats]=text
    hooks = "src/game/hooks.cpp"
    text = original[hooks]
    text = once(text,'#include "game/layout.h"', '#include "game/layout.h"\n#include "coop/probe.h"', hooks)
    text = once(text, 'X86_FUNC(sub_0017B130) {',
                 'X86_FUNC(sub_0017B130) {\n    bo_probe::ObserveAnimation(ctx.esp,base);',hooks)
    pending[hooks]=text

    # Co-op/Room must be a genuine subpage of BLACK's existing game OPTIONS
    # Flash menu, not a separate Android overlay. This is intentionally
    # read-only until ACTOR/COMBAT/NET integration has real device evidence.
    menu = "src/game/options_menu.cpp"
    text = original[menu]
    text = once(text,
        'const char* const kCaptions[] = {"GRAPHICS", "TOUCH CONTROLS", "GENERAL", "MOUSE AND KEYBOARD", "CHEATS"};',
        'const char* const kCaptions[] = {"GRAPHICS", "TOUCH CONTROLS", "GENERAL", "MOUSE AND KEYBOARD", "CHEATS", "CO-OP / ROOM"};',
        menu)
    text = once(text,
        'const Row kPageRow{app::rows::Kind::Value, -1, "PAGE"};',
        '''const Row kPageRow{app::rows::Kind::Value, -1, "PAGE"};
// Safe discovery-only page. No host/join network work or guest actor changes.
constexpr Row kRoomRows[] = {
    {app::rows::Kind::Heading, -1, "ROOM / CO-OP"},
    {app::rows::Kind::Value, -101, "ROOM STATUS"},
    {app::rows::Kind::Value, -102, "HOST ROOM"},
    {app::rows::Kind::Value, -103, "JOIN ROOM"},
    {app::rows::Kind::Value, -104, "PLAYERS"},
    {app::rows::Kind::Value, -105, "NETWORK"},
    {app::rows::Kind::Heading, -1, "MULTIPLAYER IN DEVELOPMENT"},
};
bool IsRoomRow(const Row* row) {
    return row && row->id <= -101 && row->id >= -105;
}''', menu)
    text = once(text,
        '''    const Row* list = p.extra ? app::rows::Controller(&count) : app::rows::Page(p.mode - 1, &count);
    for (size_t i = 0; i < count; i++) p.rows.push_back(&list[i]);
    if (!p.extra && app::rows::Restorable(p.mode - 1)) p.rows.push_back(nullptr);''',
        '''    const bool room = !p.extra && p.mode == kEntries;
    const Row* list = room ? kRoomRows : (p.extra ? app::rows::Controller(&count) : app::rows::Page(p.mode - 1, &count));
    if (room) count = std::size(kRoomRows);
    for (size_t i = 0; i < count; i++) p.rows.push_back(&list[i]);
    if (!p.extra && !room && app::rows::Restorable(p.mode - 1)) p.rows.push_back(nullptr);''',
        menu)
    text = once(text,
        '''    if (!row || IsHeading(row)) return " ";
    std::string value = app::rows::Value(*row);''',
        '''    if (!row || IsHeading(row)) return " ";
    if (IsRoomRow(row)) {
        switch (row->id) {
            case -101: return "OFFLINE / PREVIEW";
            case -102: return "NOT AVAILABLE";
            case -103: return "NOT AVAILABLE";
            case -104: return "2-4 PLANNED";
            case -105: return "LAN / VPN PLANNED";
            default: return " ";
        }
    }
    std::string value = app::rows::Value(*row);''',
        menu)
    text = once(text,
        '''bool Steppable(const Row* row) {
    if (row == &kPageRow) return true;''',
        '''bool Steppable(const Row* row) {
    if (IsRoomRow(row)) return false;
    if (row == &kPageRow) return true;''',
        menu)
    text = once(text,
        '''    const Row* row = p.rows[p.top + slot];
    if (IsHeading(row)) return;''',
        '''    const Row* row = p.rows[p.top + slot];
    if (IsRoomRow(row) || IsHeading(row)) return;''',
        menu)
    text = once(text,
        '''    const Row* row = g_page.rows[g_page.top + slot];
    if (clear) Clear(ctx, row);''',
        '''    const Row* row = g_page.rows[g_page.top + slot];
    if (IsRoomRow(row)) return; // no action until a real tested room exists
    if (clear) Clear(ctx, row);''',
        menu)
    pending[menu]=text
    for name,text in source_files.items():
        if (root/name).exists():
            raise ValueError(f"Refusing to overwrite existing source: {name}")
        pending[name]=text
    # No native/game_files.cpp, config/game.toml, CMakeLists, saves, XBE,
    # original package or release signing source changes.
    allowed=set(BASE_FILES)|set(GAME_FILES)|set(source_files)
    if set(pending)!=allowed:
        raise ValueError("Unexpected source change set")
    for name,contents in pending.items():
        dest=root/name
        dest.parent.mkdir(parents=True,exist_ok=True)
        dest.write_text(contents, encoding="utf-8",newline="\n")
    return {name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in pending}

if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("root",type=pathlib.Path)
    p.add_argument("--probe-dir",type=pathlib.Path,default=pathlib.Path(__file__).resolve().parent/"probe")
    a=p.parse_args()
    print(json.dumps({"package":PACKAGE,"changed_files":prepare(a.root,a.probe_dir)},indent=2))
