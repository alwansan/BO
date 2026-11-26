#!/usr/bin/env bash
set -euo pipefail
: "${ANDROID_HOME:?}"
: "${RUNNER_TEMP:?}"
out=$(mktemp -d "$RUNNER_TEMP/bo-arm64.XXXXXXXX")
emulator_pid=''
cleanup() {
    code=$?
    if [[ "$code" != 0 && -f "$out/emulator.log" ]]; then tail -60 "$out/emulator.log"; fi
    if [[ -n "$emulator_pid" ]]; then kill "$emulator_pid" 2>/dev/null || true; fi
    rm -f "$out/signing.p12"
}
trap cleanup EXIT
sdk="$ANDROID_HOME/cmdline-tools/latest/bin"
"$sdk/sdkmanager" --install 'platforms;android-34' 'build-tools;34.0.0' 'system-images;android-30;google_apis;x86_64' 'ndk;28.2.13676358' 'platform-tools' 'emulator' </dev/null
bt="$ANDROID_HOME/build-tools/34.0.0"
jar="$ANDROID_HOME/platforms/android-34/android.jar"
cxx="$ANDROID_HOME/ndk/28.2.13676358/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang++"
mkdir -p "$out/classes" "$out/dex" "$out/package/lib/arm64-v8a"
"$cxx" -std=c++20 -Wall -Wextra -Werror -shared -fPIC -static-libstdc++ coop/android_arm64_smoke/probe.cpp -o "$out/package/lib/arm64-v8a/libbo_arm64_smoke.so"
file "$out/package/lib/arm64-v8a/libbo_arm64_smoke.so" | tee "$out/elf.txt"
grep -q 'ARM aarch64' "$out/elf.txt"
javac --release 8 -Xlint:all,-options -Werror -cp "$jar" -d "$out/classes" coop/android_arm64_smoke/Arm64Instrumentation.java
"$bt/d8" --lib "$jar" --min-api 29 --output "$out/dex" "$out/classes/org/bocoop/arm64smoke/Arm64Instrumentation.class"
"$bt/aapt2" link -I "$jar" --manifest coop/android_arm64_smoke/AndroidManifest.xml -o "$out/unsigned.apk"
cp "$out/dex/classes.dex" "$out/package/classes.dex"
(cd "$out/package" && zip -q -r "$out/unsigned.apk" classes.dex lib)
"$bt/zipalign" -f 4 "$out/unsigned.apk" "$out/aligned.apk"
keytool -genkeypair -alias test -keyalg RSA -keysize 2048 -validity 2 -dname CN=TestFixture -storetype PKCS12 -keystore "$out/signing.p12" -storepass test-only -keypass test-only -noprompt
"$bt/apksigner" sign --ks "$out/signing.p12" --ks-pass pass:test-only --out "$out/test.apk" "$out/aligned.apk"
export ANDROID_USER_HOME="$out/android-user"
export ANDROID_EMULATOR_HOME="$ANDROID_USER_HOME"
export ANDROID_AVD_HOME="$out/avds"
mkdir -p "$ANDROID_USER_HOME" "$ANDROID_AVD_HOME"
printf 'no\n' | "$sdk/avdmanager" create avd --force -n bo_arm64 -p "$ANDROID_AVD_HOME/bo_arm64.avd" -k 'system-images;android-30;google_apis;x86_64'
if [[ -e /dev/kvm ]]; then sudo chmod 666 /dev/kvm; fi
"$ANDROID_HOME/emulator/emulator" -avd bo_arm64 -no-window -gpu swiftshader_indirect -no-audio -no-boot-anim -no-snapshot -memory 2048 -cores 2 -wipe-data > "$out/emulator.log" 2>&1 &
emulator_pid=$!
adb="$ANDROID_HOME/platform-tools/adb"
timeout -k 3s 12s "$adb" start-server
booted=false
for ((attempt=0;attempt<90;attempt++)); do
    kill -0 "$emulator_pid" 2>/dev/null || break
    completed=$(timeout -k 2s 5s "$adb" -s emulator-5554 shell getprop sys.boot_completed 2>/dev/null | tr -d '\r' || true)
    if [[ "$completed" == 1 ]]; then booted=true; break; fi
    sleep 2
done
[[ "$booted" == true ]]
# Report ABI/bridge capability only, never device IDs or user properties.
for property in ro.product.cpu.abilist ro.dalvik.vm.native.bridge; do
    printf '%s=' "$property"
    timeout -k 2s 8s "$adb" -s emulator-5554 shell getprop "$property"
done
timeout -k 5s 45s "$adb" -s emulator-5554 push "$out/test.apk" /data/local/tmp/bo-arm64-smoke.apk
timeout -k 5s 60s "$adb" -s emulator-5554 shell pm install -t -r /data/local/tmp/bo-arm64-smoke.apk
timeout -k 5s 90s "$adb" -s emulator-5554 shell am instrument -w -r org.bocoop.arm64smoke/org.bocoop.arm64smoke.Arm64Instrumentation | tee "$out/result.txt"
grep -q 'BO_ARM64_JNI_PASS:' "$out/result.txt"
grep -q 'INSTRUMENTATION_CODE: -1' "$out/result.txt"
