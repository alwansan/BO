"""Prepare a disposable upstream checkout for the separate baseline test app.

No game, native source, save or original APK is modified. Unknown anchors fail
before any write, so a newer upstream cannot silently produce a different app.
"""
import argparse
import hashlib
import json
import pathlib

PACKAGE = "org.bocoop.black.baseline"


def prepare(root):
    changes = {
        "app/build.gradle": [
            ('applicationId "com.racas.black"', f'applicationId "{PACKAGE}"'),
            ('arguments "-DANDROID_STL=c++_shared"',
             'arguments "-DANDROID_STL=c++_shared", "-DCMAKE_JOB_POOLS=bo_compile=2", "-DCMAKE_JOB_POOL_COMPILE=bo_compile"'),
        ],
        "app/src/main/AndroidManifest.xml": [
            ('android:name=".BlackActivity"', 'android:name="com.racas.black.BlackActivity"'),
            ('<uses-permission android:name="android.permission.MANAGE_EXTERNAL_STORAGE" />', ''),
            ('<uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" android:maxSdkVersion="32" />', ''),
            ('<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" android:maxSdkVersion="29" />', ''),
        ],
        "app/src/main/res/values/strings.xml": [
            ('<string name="app_name">Black Android</string>',
             '<string name="app_name">BLACK Baseline - NO CO-OP</string>'),
        ],
        "app/src/main/java/com/racas/black/UpdateChecker.java": [
            ('static void check(Context context, boolean manual, Listener listener) {',
             'static void check(Context context, boolean manual, Listener listener) {\n'
             f'        if (context.getPackageName().equals("{PACKAGE}")) {{\n'
             '            new Handler(Looper.getMainLooper()).post(() -> listener.onResult(null, manual));\n'
             '            return;\n'
             '        }'),
        ],
        "app/src/main/java/com/racas/black/BlackActivity.java": [
            ('private void askAllFilesAccess() {',
             'private void askAllFilesAccess() {\n'
             f'        if (getPackageName().equals("{PACKAGE}")) {{\n'
             '            dialog = new AlertDialog.Builder(this)\n'
             '                    .setTitle("BLACK Baseline")\n'
             '                    .setMessage("Choose your BLACK disc image (.iso). This test app uses its own private copy.")\n'
             '                    .setPositiveButton("Choose ISO", (d, w) -> startImport())\n'
             '                    .setNegativeButton("Not now", (d, w) -> notNow = true)\n'
             '                    .setOnDismissListener(d -> tellOfUpdate())\n'
             '                    .show();\n'
             '            return;\n'
             '        }'),
            ('File dir = hasStorageAccess()\n'
             '                ? new File(Environment.getExternalStorageDirectory(), SHARED)\n'
             '                : new File(getExternalFilesDir(null), "assets");',
             'File dir = new File(getExternalFilesDir(null), "assets");'),
        ],
    }
    pending = {}
    for name, replacements in changes.items():
        text = (root / name).read_text(encoding="utf-8")
        for old, new in replacements:
            if text.count(old) != 1:
                raise ValueError(f"Unknown or already patched upstream: {name}")
            text = text.replace(old, new)
        pending[name] = text
    for name, text in pending.items():
        (root / name).write_text(text, encoding="utf-8", newline="\n")
    return {name: hashlib.sha256((root / name).read_bytes()).hexdigest()
            for name in pending}


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("root", type=pathlib.Path)
    args = parser.parse_args()
    print(json.dumps({"package": PACKAGE, "changed_files": prepare(args.root)}, indent=2))
