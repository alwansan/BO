// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import android.content.Context;
import android.content.ContentResolver;
import android.content.ContentValues;
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

/** Persistent sanitized room events, usable from either Android device. */
public final class RoomDiagnostics {
    private RoomDiagnostics(){}
    private static final Object lock=new Object();
    private static Context app;
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
    /** Codes only. NEVER emit a user name, PIN, IP, cert, fingerprint, or key. */
    public static void event(String code){
        if(code==null||!code.matches("[A-Z0-9_]{1,70}"))return;
        synchronized(lock){
            File file=path();if(file==null)return;
            try{
                if(file.length()>49152){
                    File old=new File(file.getParentFile(),"coop_room_events.old.log");
                    if(old.exists()&&!old.delete())return;
                    if(!file.renameTo(old))return;
                }
                String row=String.format(Locale.ROOT,"%d %s%n",System.currentTimeMillis(),code);
                try(FileOutputStream out=new FileOutputStream(file,true)){
                    out.write(row.getBytes(StandardCharsets.UTF_8));
                }
            }catch(Exception ignored){}
        }
    }
    /**
     * One-shot, explicit opt-in export of event codes ONLY. Runs on a worker,
     * never on the UI/game thread; no all-files permission is requested.
     * The app never exports ISO, gameplay saves, TLS credentials or original logs.
     */
    public static boolean exportToDownloads(Context context){
        if(Build.VERSION.SDK_INT<29)return false;
        final String contents=read();
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
            return new String(bytes,StandardCharsets.UTF_8);
        }
    }
    /** One combined export keeps the previous rotated segment and current
     *  safe codes. No extra owner phone test is needed for the earlier events. */
    public static String read(){
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
