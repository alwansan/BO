package com.racas.black.coop;
import java.io.IOException;
import java.util.*;
public final class RoomGuestHostWorldSourceTest {
    static void ok(boolean b){if(!b)throw new AssertionError("Guest host source");}
    interface Attempt{void run()throws Exception;}
    static void bad(Attempt a)throws Exception{
        try{a.run();throw new AssertionError("Accepted untrusted host view");}
        catch(IOException expected){}
    }
    static RoomWire.State state(byte[] session,int self,int epoch)throws IOException{
        List<RoomWire.Player> players=Arrays.asList(
          new RoomWire.Player(1,"Host"),new RoomWire.Player(2,"Two"),
          new RoomWire.Player(3,"Three"));
        return RoomWire.parseState(RoomWire.decode(RoomWire.state(
            RoomWire.ROSTER,session,session,self,epoch,"Campaign",players)));
    }
    public static void main(String[]args)throws Exception{
        byte[] session=new byte[16];Arrays.fill(session,(byte)12);
        RoomStoryWire.Ticket ticket=new RoomStoryWire.Ticket(session,11,9,2,0,17,7);
        RoomWire.State view=state(session,3,9);
        List<RoomWorldSnapshotWire.Avatar> avatars=Arrays.asList(
           new RoomWorldSnapshotWire.Avatar(1,-100,20,30,0,0),
           new RoomWorldSnapshotWire.Avatar(2,111,222,333,9000,1),
           new RoomWorldSnapshotWire.Avatar(3,444,555,666,-9000,2));
        RoomWorldSnapshotWire.Snapshot snap=RoomWorldSnapshotWire.create(ticket,2,avatars);
        int[] packed=RoomGuestHostWorldSource.pack(ticket,snap,view);
        ok(packed.length==22&&packed[0]==17&&packed[1]==2&&packed[2]==3&&packed[3]==3);
        ok(packed[4]==1&&packed[5]==-100);
        ok(packed[10]==2&&packed[11]==111&&packed[14]==9000&&packed[15]==1);
        ok(packed[16]==3&&packed[17]==444&&packed[20]==-9000&&packed[21]==2);
        bad(()->RoomGuestHostWorldSource.pack(null,snap,view));
        bad(()->RoomGuestHostWorldSource.pack(ticket,null,view));
        bad(()->RoomGuestHostWorldSource.pack(ticket,snap,null));
        bad(()->RoomGuestHostWorldSource.pack(ticket,snap,state(session,1,9)));
        bad(()->RoomGuestHostWorldSource.pack(ticket,snap,state(session,3,10)));
        byte[] otherSession=new byte[16];Arrays.fill(otherSession,(byte)4);
        bad(()->RoomGuestHostWorldSource.pack(ticket,snap,state(otherSession,3,9)));
        RoomStoryWire.Ticket newEpoch=new RoomStoryWire.Ticket(session,11,9,2,0,18,7);
        bad(()->RoomGuestHostWorldSource.pack(newEpoch,snap,view));
        RoomStoryWire.Ticket fewer=new RoomStoryWire.Ticket(session,11,9,2,0,17,3);
        RoomWorldSnapshotWire.Snapshot partial=RoomWorldSnapshotWire.create(fewer,2,
          Arrays.asList(avatars.get(0),avatars.get(1)));
        bad(()->RoomGuestHostWorldSource.pack(fewer,partial,view)); // self not in story
        RoomWorldSnapshotWire.Snapshot missing=RoomWorldSnapshotWire.create(ticket,3,
          Arrays.asList(avatars.get(0),avatars.get(2)));
        bad(()->RoomGuestHostWorldSource.pack(ticket,missing,view));
        System.out.println("GUEST_AUTHENTICATED_HOST_WORLD_PACK_PASS: real TLS-derived snapshot and exact ticket/roster, no NPC or READY");
    }
}
