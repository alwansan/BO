// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import android.content.Context;
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
    public static String read(){
        synchronized(lock){
            File file=path();
            if(file==null||!file.isFile())return "No BLACK Co-op room log exists yet.";
            if(file.length()>65536)return "Room log exceeds safe export size.";
            try(FileInputStream in=new FileInputStream(file)){
                byte[] bytes=new byte[(int)file.length()];
                int count=0;
                while(count<bytes.length){
                    int n=in.read(bytes,count,bytes.length-count);
                    if(n<=0)break;
                    count+=n;
                }
                return new String(bytes,0,count,StandardCharsets.UTF_8);
            }catch(Exception ignored){return "Unable to read BLACK Co-op room log.";}
        }
    }
}
