// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.IOException;
import java.security.MessageDigest;
import java.util.List;

/**
 * Authenticated HOST snapshot -> bounded game-thread integers on a GUEST.
 *
 * This helper has NO Android dependency. Caller MUST use
 * RoomSession.readHostWorldFromGameThread(), which checks exact original
 * game-thread identity and only reads a COMMITTED live HOST TLS snapshot.
 * Serialized ints are values, never BLACK engine pointers or bot IDs.
 * An engine renderer/scene launcher is NOT implemented by this helper.
 */
public final class RoomGuestHostWorldSource {
    private RoomGuestHostWorldSource(){}
    public static int[] pack(RoomStoryWire.Ticket ticket,
                             RoomWorldSnapshotWire.Snapshot snap,
                             RoomWire.State state)throws IOException{
        if(ticket==null||snap==null||state==null||state.self<2||state.self>4||
           !ticket.includes(state.self)||!snap.matches(ticket)||
           state.revision!=ticket.rosterRevision||
           !MessageDigest.isEqual(state.session,ticket.session())||
           snap.tick<=0||snap.avatars.size()<2||snap.avatars.size()>4)
            throw new IOException("No trusted host world for this guest and ticket");
        final List<RoomWorldSnapshotWire.Avatar> avatars=snap.avatars;
        int participants=0;
        int[] numbers=new int[4+avatars.size()*6];
        numbers[0]=ticket.sceneEpoch;numbers[1]=snap.tick;
        numbers[2]=state.self;numbers[3]=avatars.size();
        for(int i=0;i<avatars.size();i++){
            RoomWorldSnapshotWire.Avatar avatar=avatars.get(i);
            if(avatar.playerId<1||avatar.playerId>4||
               !ticket.includes(avatar.playerId)||
               (participants&(1<<(avatar.playerId-1)))!=0)
                throw new IOException("Host world includes a nonparticipant");
            boolean rosterMember=false;
            for(RoomWire.Player p:state.players)
                if(p.id==avatar.playerId){rosterMember=true;break;}
            if(!rosterMember)throw new IOException("Host world includes a non-room peer");
            participants|=1<<(avatar.playerId-1);
            int at=4+i*6;
            numbers[at]=avatar.playerId;
            numbers[at+1]=avatar.xCm;numbers[at+2]=avatar.yCm;
            numbers[at+3]=avatar.zCm;numbers[at+4]=avatar.yawCentiDeg;
            numbers[at+5]=avatar.flags;
        }
        // The host must include exactly the current committed human roster.
        // Never partially hide a missing human or reuse an earlier game stage.
        if(participants!=ticket.participants)
            throw new IOException("Incomplete current committed host world");
        return numbers;
    }
}
