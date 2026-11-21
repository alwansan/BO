#!/usr/bin/env bash
set -euo pipefail
: "${ANDROID_HOME:?Android SDK required}"
: "${RUNNER_TEMP:?Temporary runner directory required}"
out="$RUNNER_TEMP/black-tls-platform"
mkdir -p "$out/classes" "$out/dex"
cleanup() {
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
    coop/lobby/platform/PlatformInstrumentation.java
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
# All files are synthetic test code. No game data or production identities enter this job.
printf 'no\n' | "$sdk/avdmanager" create avd --force -n black_tls -k 'system-images;android-29;default;x86_64'
if [[ -e /dev/kvm ]]; then sudo chmod 666 /dev/kvm; fi
"$ANDROID_HOME/emulator/emulator" -avd black_tls -no-window -gpu swiftshader_indirect -no-audio -no-boot-anim -no-snapshot -memory 2048 -cores 2 -wipe-data > "$out/emulator.log" 2>&1 &
emulator_pid=$!
adb="$ANDROID_HOME/platform-tools/adb"
timeout 120s "$adb" -s emulator-5554 wait-for-device
booted=false
for ((i=0;i<120;i++)); do
    if [[ "$("$adb" -s emulator-5554 shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')" == 1 ]]; then booted=true; break; fi
    if ! kill -0 "$emulator_pid" 2>/dev/null; then tail -60 "$out/emulator.log"; exit 1; fi
    sleep 2
done
[[ "$booted" == true ]]
"$adb" -s emulator-5554 install -t "$out/test.apk"
timeout 60s "$adb" -s emulator-5554 shell am instrument -w -r org.bocoop.black.tlsplatform/com.racas.black.coop.PlatformInstrumentation | tee "$out/result.txt"
if command -v rg >/dev/null 2>&1; then
    passed=$(rg -c 'TLS_PLATFORM_PASS' "$out/result.txt" || true)
else
    passed=$(grep -c 'TLS_PLATFORM_PASS' "$out/result.txt" || true)
fi
if [[ "$passed" != 1 ]]; then
    "$adb" -s emulator-5554 logcat -d -s BlackTlsPlatform:E AndroidRuntime:E
    exit 1
fi
