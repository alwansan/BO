// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.provider.MediaStore;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

// Opt-in only: runs AFTER Start capture, never reads private data of other apps.
// Copies only diagnostic files from this app's own storage to shared Downloads.
// Android 10+ MediaStore Downloads needs NO MANAGE_EXTERNAL_STORAGE permission.
// Does not include ISO, saved games, assets, network credentials or shader caches.
final class ProbeLogExporter {
    private static final String TAG = "BOProbeExport";
    private static final long INTERVAL_MS = 10000L;
    private static final long MAX_RUN_MS = 8 * 60 * 1000L;
    private static final long MAX_CAPTURE_BYTES = 2 * 1024 * 1024L;
    private static final long MAX_GAME_LOG_BYTES = 4 * 1024 * 1024L;
    private static final String BASE = Environment.DIRECTORY_DOWNLOADS + "/Games/cl/black log/logs/";

    private final Context app;
    private final Handler main = new Handler(Looper.getMainLooper());
    private final ExecutorService io = Executors.newSingleThreadExecutor(r -> {
        Thread thread = new Thread(r, "bo-probe-export");
        thread.setDaemon(true);
        return thread;
    });
    private final AtomicBoolean busy = new AtomicBoolean(false);
    private final Map<String, Uri> ownFiles = new HashMap<>(); // only touched by io thread
    private volatile boolean started;
    private long startedAt;
    private String destination;

    ProbeLogExporter(Context app) {
        this.app = app.getApplicationContext();
    }

    private final Runnable ticker = new Runnable() {
        @Override public void run() {
            if (!started) return;
            snapshot();
            if (android.os.SystemClock.elapsedRealtime() - startedAt < MAX_RUN_MS)
                main.postDelayed(this, INTERVAL_MS);
            else started = false;
        }
    };

    // Called on UI thread after the user explicitly accepts the public export.
    void start() {
        if (started) return;
        started = true;
        startedAt = android.os.SystemClock.elapsedRealtime();
        String stamp = new SimpleDateFormat("yyyyMMdd-HHmmss-SSS", Locale.US)
                .format(new Date());
        destination = BASE + "BLACK-Probe-" + stamp + "/";
        snapshot();
        main.postDelayed(ticker, INTERVAL_MS);
    }

    // Safe to call from Activity.onPause as well; never copies on the game/UI thread.
    void snapshot() {
        if (!started || !busy.compareAndSet(false, true)) return;
        try {
            io.execute(() -> {
                try { exportNow(); }
                finally { busy.set(false); }
            });
        } catch (RuntimeException ignored) {
            busy.set(false);
        }
    }

    private void exportNow() {
        File root = app.getExternalFilesDir(null);
        if (root == null) return;
        File logs = new File(root, "logs");
        copyBounded(new File(logs, "coop_probe.jsonl"), "coop_probe.txt", MAX_CAPTURE_BYTES);
        copyBounded(new File(logs, "black.log"), "black_log.txt", MAX_GAME_LOG_BYTES);
        // No world image, personal saves, shader cache, hdd, or arbitrary traversal.
    }

    private void copyBounded(File input, String outputName, long limit) {
        long length = input.length();
        if (!input.isFile() || length == 0 || length > limit) return;
        ContentResolver resolver = app.getContentResolver();
        Uri uri = ownFiles.get(outputName);
        boolean created = false;
        try {
            if (uri == null) {
                ContentValues values = new ContentValues();
                values.put(MediaStore.MediaColumns.DISPLAY_NAME, outputName);
                values.put(MediaStore.MediaColumns.MIME_TYPE, "text/plain");
                values.put(MediaStore.MediaColumns.RELATIVE_PATH, destination);
                values.put(MediaStore.MediaColumns.IS_PENDING, 1);
                uri = resolver.insert(MediaStore.Downloads.EXTERNAL_CONTENT_URI, values);
                if (uri == null) return;
                created = true;
            }
            try (FileInputStream src = new FileInputStream(input);
                 OutputStream dest = resolver.openOutputStream(uri, "rwt")) {
                if (dest == null) throw new IOException("no output stream");
                byte[] buffer = new byte[16384];
                long copied = 0;
                int n;
                while ((n = src.read(buffer)) != -1) {
                    copied += n;
                    if (copied > limit) throw new IOException("size changed during export");
                    dest.write(buffer, 0, n);
                }
                dest.flush();
            }
            if (created) {
                ContentValues ready = new ContentValues();
                ready.put(MediaStore.MediaColumns.IS_PENDING, 0);
                resolver.update(uri, ready, null, null);
                ownFiles.put(outputName, uri);
            }
        } catch (IOException | SecurityException | IllegalArgumentException e) {
            // The next snapshot can try again. Only our newly inserted item is
            // discarded; no user-created files are deleted.
            if (created && uri != null) {
                try { resolver.delete(uri, null, null); }
                catch (RuntimeException ignored) { /* fail closed */ }
            } else if (!created) {
                ownFiles.remove(outputName);
            }
            Log.w(TAG, "Could not export " + outputName + "; original local log preserved");
        }
    }

    // The original in-app log is never deleted or chmod'ed. Shared copies
    // are accessible to file managers and Termux, so opt-in is explained in UI.
}
