#!/usr/bin/env bash
set -euo pipefail
: "${ANDROID_HOME:?Android SDK required}"
: "${RUNNER_TEMP:?Temporary runner directory required}"
out="$RUNNER_TEMP/black-tls-platform"
mkdir -p "$out/classes" "$out/dex"
cleanup() {
    code=$?
    if [[ "$code" != 0 && -f "$out/emulator.log" ]]; then tail -100 "$out/emulator.log"; fi
    if [[ -n "${emulator_pid:-}" ]]; then kill "$emulator_pid" 2>/dev/null || true; fi
    rm -f "$out/signing.p12"
}
trap cleanup EXIT
sdk="$ANDROID_HOME/cmdline-tools/latest/bin"
"$sdk/sdkmanager" --install 'platforms;android-34' 'build-tools;34.0.0' 'system-images;android-29;default;x86_64' 'platform-tools' 'emulator' </dev/null
jar="$ANDROID_HOME/platforms/android-34/android.jar"
bt="$ANDROID_HOME/build-tools/34.0.0"
javac --release 8 -Xlint:all,-options -Werror -cp "$jar" -d "$out/classes" \
    coop/lobby/java/com/racas/black/coop/*.java \
    coop/lobby/android/com/racas/black/coop/RoomIdentity.java \
    coop/lobby/android/com/racas/black/coop/RoomAppRooms.java \
    coop/lobby/android/com/racas/black/coop/RoomDiagnostics.java \
    coop/lobby/android/com/racas/black/coop/RoomOverlay.java \
    coop/lobby/platform/PlatformInstrumentation.java \
    coop/lobby/platform/RoomHarnessActivity.java \
    coop/lobby/platform/RoomLobbyActivity.java
if command -v rg >/dev/null 2>&1; then
    rg --files "$out/classes" -g '*.class' > "$out/class-list.txt"
else
    find "$out/classes" -type f -name '*.class' > "$out/class-list.txt"
fi
mapfile -t classes < "$out/class-list.txt"
(( ${#classes[@]} > 0 ))
"$bt/d8" --lib "$jar" --min-api 29 --output "$out/dex" "${classes[@]}"
"$bt/aapt2" link -I "$jar" --manifest coop/lobby/platform/AndroidManifest.xml -o "$out/unsigned.apk"
zip -q -j "$out/unsigned.apk" "$out/dex/classes.dex"
"$bt/zipalign" -f 4 "$out/unsigned.apk" "$out/aligned.apk"
keytool -genkeypair -alias test -keyalg RSA -keysize 2048 -validity 2 -dname CN=TestFixture -storetype PKCS12 -keystore "$out/signing.p12" -storepass test-only -keypass test-only -noprompt
"$bt/apksigner" sign --ks "$out/signing.p12" --ks-pass pass:test-only --key-pass pass:test-only --out "$out/test.apk" "$out/aligned.apk"
# Use one explicit AVD registry for Java avdmanager and the native emulator.
export ANDROID_USER_HOME="$out/android-user"
export ANDROID_EMULATOR_HOME="$ANDROID_USER_HOME"
export ANDROID_AVD_HOME="$out/avds"
mkdir -p "$ANDROID_USER_HOME" "$ANDROID_AVD_HOME"
# All files are synthetic test code. No game data or production identities enter this job.
printf 'no\n' | "$sdk/avdmanager" create avd --force -n black_tls -p "$ANDROID_AVD_HOME/black_tls.avd" -k 'system-images;android-29;default;x86_64'
"$ANDROID_HOME/emulator/emulator" -list-avds | tee "$out/avd-list.txt"
grep -qx 'black_tls' "$out/avd-list.txt"
if [[ -e /dev/kvm ]]; then sudo chmod 666 /dev/kvm; ls -l /dev/kvm; fi
"$ANDROID_HOME/emulator/emulator" -accel-check || true
"$ANDROID_HOME/emulator/emulator" -avd black_tls -no-window -gpu swiftshader_indirect -no-audio -no-boot-anim -no-snapshot -memory 2048 -cores 2 -wipe-data > "$out/emulator.log" 2>&1 &
emulator_pid=$!
adb="$ANDROID_HOME/platform-tools/adb"
# ADB can hang indefinitely if a launched emulator never becomes ONLINE.
# Every adb invocation MUST be bounded with TERM and a hard KILL fallback.
timeout -k 3s 12s "$adb" start-server
booted=false
for ((i=0;i<65;i++)); do
    if ! kill -0 "$emulator_pid" 2>/dev/null; then
        echo "Emulator exited before boot"; tail -100 "$out/emulator.log"; exit 1
    fi
    state="$(timeout -k 2s 5s "$adb" -s emulator-5554 get-state 2>/dev/null || true)"
    if [[ "$state" == device ]]; then
        completed="$(timeout -k 2s 5s "$adb" -s emulator-5554 shell getprop sys.boot_completed 2>/dev/null | tr -d '\r' || true)"
        if [[ "$completed" == 1 ]]; then booted=true; break; fi
    fi
    if (( i % 10 == 0 )); then
        echo "Emulator boot poll $i/65: ADB state=${state:-unavailable}"
        timeout -k 2s 5s "$adb" devices -l || true
        tail -12 "$out/emulator.log" || true
    fi
    sleep 2
done
if [[ "$booted" != true ]]; then
    echo "Emulator failed to boot in bounded ADB poll"
    tail -120 "$out/emulator.log"
    exit 1
fi
# Android SDK adb may stall in its incremental/streaming install server
# even though the emulator has booted. Stage as an ordinary shell-owned file
# and ask PackageManager to install it directly. No game assets are involved.
remote=/data/local/tmp/bo_coop_tls_platform.apk
timeout -k 5s 40s "$adb" -s emulator-5554 push "$out/test.apk" "$remote"
installed="$(timeout -k 5s 60s "$adb" -s emulator-5554 shell pm install -r -t "$remote" | tr -d '\r')"
printf '%s\n' "$installed"
[[ "$installed" == *Success* ]]
timeout -k 3s 10s "$adb" -s emulator-5554 shell rm -f "$remote" || true
timeout -k 5s 90s "$adb" -s emulator-5554 shell am instrument -w -r org.bocoop.black.tlsplatform/com.racas.black.coop.PlatformInstrumentation | tee "$out/result.txt"
if command -v rg >/dev/null 2>&1; then
    passed=$(rg -c 'TLS_PLATFORM_PASS' "$out/result.txt" || true)
else
    passed=$(grep -c 'TLS_PLATFORM_PASS' "$out/result.txt" || true)
fi
if [[ "$passed" != 1 ]]; then
    timeout -k 3s 10s "$adb" -s emulator-5554 logcat -d -s BlackTlsPlatform:E AndroidRuntime:E || true
    exit 1
fi
