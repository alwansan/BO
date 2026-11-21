// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

/**
 * Application-process lobby owner for the isolated game room-net build.
 * No gameplay actor or guest pointer is stored here.
 */
public final class RoomAppRooms {
    private static RoomSession active;
    private RoomAppRooms(){}
    public static synchronized RoomSession obtain(){
        if(active==null){active=RoomSession.create();active.setReporter(RoomDiagnostics::event);}
        return active;
    }
    public static synchronized void shutdown(){
        if(active!=null){active.close();active=null;}
    }
    /** Game thread calls through the Activity with a bounded 500ms JNI cache. */
    public static String statusLine(int id){
        RoomSession.Status state=obtain().snapshot();
        int humans=state.roster==null?0:state.roster.players.size();
        switch(id){
            case -101:
                switch(state.phase){
                    case HOSTING:return "HOSTING";
                    case JOINED:return "JOINED";
                    case CONNECTING:return "CONNECTING";
                    case ERROR:return "DISCONNECTED";
                    default:return "OFFLINE";
                }
            case -102:return "OPEN HOST FORM";
            case -103:return "OPEN JOIN FORM";
            case -104:return humans+"/4 HUMANS";
            case -105:return "PINNED TLS LAN / VPN";
            default:return "";
        }
    }
}
