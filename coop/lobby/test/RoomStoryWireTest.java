package com.racas.black.coop;
import java.io.*;
import java.util.*;
/** Strict story control protocol, no simulated game scene or actor. */
public final class RoomStoryWireTest {
    static void ok(boolean good){if(!good)throw new AssertionError("Story wire invariant");}
    interface Checked {void run()throws Exception;}
    static void bad(Checked action)throws Exception {
        try{action.run();}
        catch(IOException expected){return;}
        throw new AssertionError("Accepted invalid story frame");
    }
    public static void main(String[]args)throws Exception{
        byte[] session=new byte[16];Arrays.fill(session,(byte)17);
        var first=new RoomStoryWire.Ticket(session,33,5,0,0,1,3);
        session[0]^=12;
        ok(first.session()[0]==17);
        first.session()[0]=0;
        ok(first.session()[0]==17);
        ok(first.includes(1)&&first.includes(2)&&!first.includes(3));
        ok(!first.includes(5));
        for(int type=RoomWire.STORY_PREPARE;type<=RoomWire.STORY_ABORT;type++){
            byte[] raw=RoomStoryWire.encode(type,first);
            RoomWire.Packet framed=RoomWire.decode(raw);
            ok(framed.type==type);
            ok(first.same(RoomStoryWire.parse(framed)));
            ok(raw.length==44); // 8B frame + 36B bounded ticket
            byte[] badLength=Arrays.copyOf(raw,raw.length+1);
            bad(()->RoomWire.decode(badLength));
            byte[] badMission=raw.clone();badMission[8+16+8+4]=9;
            bad(()->RoomStoryWire.parse(RoomWire.decode(badMission)));
            byte[] badMask=raw.clone();badMask[badMask.length-1]=1;
            bad(()->RoomStoryWire.parse(RoomWire.decode(badMask)));
            byte[] badEpoch=raw.clone();
            Arrays.fill(badEpoch,8+16+8+4+1+2,8+16+8+4+1+2+4,(byte)0);
            bad(()->RoomStoryWire.parse(RoomWire.decode(badEpoch)));
            byte[] badVersion=raw.clone();badVersion[4]=2;
            bad(()->RoomWire.decode(badVersion));
        }
        bad(()->RoomStoryWire.encode(RoomWire.PING,first));
        bad(()->new RoomStoryWire.Ticket(new byte[15],1,1,0,0,1,3));
        bad(()->new RoomStoryWire.Ticket(new byte[16],0,1,0,0,1,3));
        bad(()->new RoomStoryWire.Ticket(new byte[16],1,1,8,0,1,3));
        bad(()->new RoomStoryWire.Ticket(new byte[16],1,1,0,65536,1,3));
        bad(()->new RoomStoryWire.Ticket(new byte[16],1,1,0,0,0,3));
        bad(()->new RoomStoryWire.Ticket(new byte[16],1,1,0,0,1,1));
        bad(()->new RoomStoryWire.Ticket(new byte[16],1,1,0,0,1,16));
        var other=new RoomStoryWire.Ticket(new byte[16],33,5,0,0,1,3);
        ok(!first.same(other));
        System.out.println("STORY_WIRE_PASS: zero-based mission 0, stage0, strict binary ticket, session/roster/nonce, no downgrade");
    }
}
