// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import android.content.Context;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.pm.PackageInfo;
import java.security.MessageDigest;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import java.io.OutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;
import java.util.Locale;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.atomic.AtomicInteger;

/** Persistent sanitized room events, usable from either Android device. */
public final class RoomDiagnostics {
    private RoomDiagnostics(){}
    private static final Object lock=new Object();
    private static Context app;
    // Never open/rotate/write the on-device file on any game, UI or TLS thread.
    // Bounded 2048-event backlog (~150 KiB worst case); overload fails closed
    // with one aggregate marker, not an unbounded queue or per-frame writes.
    private static final AtomicInteger dropped=new AtomicInteger();
    private static final ThreadPoolExecutor writer=new ThreadPoolExecutor(
        1,1,0,TimeUnit.MILLISECONDS,new ArrayBlockingQueue<>(2048),
        job->{Thread t=new Thread(job,"bo-diagnostic-io");t.setDaemon(true);return t;},
        new ThreadPoolExecutor.AbortPolicy());
    public static void init(Context context){
        synchronized(lock){app=context.getApplicationContext();}
    }
    private static File path(){
        if(app==null)return null;
        File base=app.getExternalFilesDir(null);
        if(base==null)base=app.getFilesDir();
        File directory=new File(base,"logs");
        if(!directory.exists()&&!directory.mkdirs())return null;
        return new File(directory,"coop_room_events.log");
    }
    /** Codes only. NEVER emit a user name, PIN, IP, cert, fingerprint, or key.
     *  Pure in-memory bounded enqueue; safe to invoke from game-frame callbacks. */
    public static void event(String code){
        if(!safeCode(code))return;
        final long timestamp=System.currentTimeMillis();
        try{
            writer.execute(()->writeEventOffGameThread(code,timestamp));
        }catch(RejectedExecutionException overloaded){
            dropped.incrementAndGet(); // no IO, no callbacks, no user data
        }
    }
    /** Fixed original post-frame diagnostics, shared with the export fixture. */
    public static void originalPostFrameEvent(int code){
        switch(code){
            case 2130: event("ORIGINAL_POST_1F1080_GAME_FRAME_STABLE_SOURCE_ONLY_NO_PLAYER2"); break;
            case 2131: event("ORIGINAL_UNIT_ORDINALS_WORLD_LIST_MEMBERSHIP_OBSERVED"); break;
            case 2132: event("ORIGINAL_UNIT_EARLY_TEARDOWN_WORLD_LIST_REMOVAL_OBSERVED"); break;
            case 2133: event("ORIGINAL_SAME_UNIT_WORLD_LIST_RELINK_OBSERVED"); break;
            case 2134: event("ORIGINAL_GUEST_OBJECTIVE_ACTIVATION_READBACK_VERIFIED"); break;
            case 2135: event("ORIGINAL_GUEST_OBJECTIVE_ACTIVATION_READBACK_FAILED"); break;
            default: break;
        }
    }
    private static boolean safeCode(String code){
        if(code==null||code.isEmpty()||code.length()>70)return false;
        for(int i=0;i<code.length();i++){
            char ch=code.charAt(i);
            if(!(ch>='A'&&ch<='Z')&&!(ch>='0'&&ch<='9')&&ch!='_')return false;
        }
        return true;
    }
    private static void writeEventOffGameThread(String code,long timestamp){
        synchronized(lock){
            if(dropped.getAndSet(0)>0)append("ROOM_LOG_EVENTS_DROPPED",timestamp);
            append(code,timestamp);
        }
    }
    private static void append(String code,long timestamp){
        File file=path();if(file==null)return;
        try{
            if(file.length()>49152){
                File old=new File(file.getParentFile(),"coop_room_events.old.log");
                if(old.exists()&&!old.delete())return;
                if(!file.renameTo(old))return;
            }
            String row=String.format(Locale.ROOT,"%d %s%n",timestamp,code);
            try(FileOutputStream out=new FileOutputStream(file,true)){
                out.write(row.getBytes(StandardCharsets.UTF_8));
            }
        }catch(Exception ignored){}
    }
    /** Explicit export/read barrier. Never used by ordinary event producer. */
    private static boolean waitForWriter(){
        if(Thread.currentThread().getName().equals("bo-diagnostic-io"))return true;
        CountDownLatch done=new CountDownLatch(1);
        try{
            if(!writer.getQueue().offer(done::countDown,2,TimeUnit.SECONDS))return false;
            // With a lazy executor the queue needs its first worker started.
            writer.prestartCoreThread();
            return done.await(5,TimeUnit.SECONDS);
        }catch(InterruptedException e){
            Thread.currentThread().interrupt();return false;
        }
    }
    /**
     * One-shot, explicit opt-in export of event codes ONLY. Runs on a worker,
     * never on the UI/game thread; no all-files permission is requested.
     * The app never exports ISO, gameplay saves, TLS credentials or original logs.
     */
    public static boolean exportToDownloads(Context context){
        if(Build.VERSION.SDK_INT<29)return false;
        final String contents;
        try{contents=exportContents(context);}
        catch(Exception unavailable){return false;}
        if(contents.startsWith("No BLACK Co-op room log")||
           contents.startsWith("Room log exceeds")||
           contents.startsWith("Unable to read"))return false;
        final ContentResolver resolver=context.getApplicationContext().getContentResolver();
        final ContentValues metadata=new ContentValues();
        final String fileName="BLACK-Room-"+System.currentTimeMillis()+"-events.txt";
        metadata.put(MediaStore.MediaColumns.DISPLAY_NAME,fileName);
        metadata.put(MediaStore.MediaColumns.MIME_TYPE,"text/plain");
        metadata.put(MediaStore.MediaColumns.RELATIVE_PATH,
            Environment.DIRECTORY_DOWNLOADS+"/Games/cl/black log/logs/");
        metadata.put(MediaStore.MediaColumns.IS_PENDING,1);
        Uri created=null;
        try{
            created=resolver.insert(MediaStore.Downloads.EXTERNAL_CONTENT_URI,metadata);
            if(created==null)return false;
            try(OutputStream stream=resolver.openOutputStream(created,"w")){
                if(stream==null)throw new java.io.IOException("MediaStore stream unavailable");
                byte[] bytes=contents.getBytes(StandardCharsets.UTF_8);
                if(bytes.length>131072)throw new java.io.IOException("Oversized diagnostics");
                stream.write(bytes);stream.flush();
            }
            ContentValues ready=new ContentValues();
            ready.put(MediaStore.MediaColumns.IS_PENDING,0);
            if(resolver.update(created,ready,null,null)!=1)
                throw new java.io.IOException("Failed to publish diagnostics");
            return true;
        }catch(Exception ignored){
            if(created!=null)try{resolver.delete(created,null,null);}catch(Exception alsoIgnored){}
            return false;
        }
    }
    /** Called on the explicit export worker, never on a frame callback.
     * The APK digest identifies the exact installed base APK even when two
     * candidates share a version name. No device or network identifiers enter it. */
    static String exportContents(Context context)throws Exception{
        String events=read();
        if(!events.matches("(?s)[0-9]{13} [A-Z0-9_]{1,70}\\n.*"))
            throw new java.io.IOException("Room events unavailable");
        Context application=context.getApplicationContext();
        String packageName=application.getPackageName();
        PackageInfo info=application.getPackageManager().getPackageInfo(packageName,0);
        String version=info.versionName==null?"UNKNOWN":info.versionName;
        if(!packageName.matches("[A-Za-z0-9_.]{1,200}")||
           !version.matches("[A-Za-z0-9_.+() -]{1,120}"))
            throw new java.io.IOException("Unsupported package metadata");
        MessageDigest digest=MessageDigest.getInstance("SHA-256");
        try(FileInputStream input=new FileInputStream(application.getApplicationInfo().sourceDir)){
            byte[] block=new byte[65536];
            int count;
            while((count=input.read(block))!=-1)digest.update(block,0,count);
        }
        StringBuilder hash=new StringBuilder(64);
        for(byte value:digest.digest())hash.append(String.format(Locale.ROOT,"%02x",value&255));
        return "BO_ROOM_DIAGNOSTICS_SCHEMA=1\n"+
            "PACKAGE="+packageName+"\n"+
            "VERSION_NAME="+version+"\n"+
            "VERSION_CODE="+info.getLongVersionCode()+"\n"+
            "BASE_APK_SHA256="+hash+"\n\n"+events;
    }
    private static String readSegment(File segment)throws java.io.IOException{
        if(segment==null||!segment.isFile())return "";
        // Only files written by our code-filtered reporter are eligible.
        if(segment.length()>65536)throw new java.io.IOException("Oversized room log segment");
        try(FileInputStream in=new FileInputStream(segment)){
            byte[] bytes=new byte[(int)segment.length()];
            int count=0;
            while(count<bytes.length){
                int n=in.read(bytes,count,bytes.length-count);
                if(n<0)break;
                if(n==0)throw new java.io.IOException("Short room diagnostics read");
                count+=n;
            }
            if(count!=bytes.length)throw new java.io.IOException("Truncated room log");
            String text=new String(bytes,StandardCharsets.UTF_8);
            // External-files logs might be modified by other software.
            // Never copy arbitrary lines into a supposedly sanitized export.
            for(String row:text.split("\\n",-1)){
                if(row.isEmpty())continue;
                if(!row.matches("[0-9]{13} [A-Z0-9_]{1,70}"))
                    throw new java.io.IOException("Invalid room log content");
            }
            return text;
        }
    }
    /** One combined export keeps the previous rotated segment and current
     *  safe codes. No extra owner phone test is needed for the earlier events. */
    public static String read(){
        if(!waitForWriter())return "Unable to read BLACK Co-op room log.";
        synchronized(lock){
            File current=path();
            if(current==null)return "No BLACK Co-op room log exists yet.";
            File older=new File(current.getParentFile(),"coop_room_events.old.log");
            try{
                String previous=readSegment(older);
                String active=readSegment(current);
                if(previous.isEmpty()&&active.isEmpty())
                    return "No BLACK Co-op room log exists yet.";
                String joined=previous+active;
                if(joined.getBytes(StandardCharsets.UTF_8).length>131072)
                    return "Room log exceeds safe export size.";
                return joined;
            }catch(java.io.IOException error){
                return "Unable to read BLACK Co-op room log.";
            }
        }
    }
}
