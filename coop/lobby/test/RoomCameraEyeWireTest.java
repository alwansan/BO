package com.racas.black.coop;
import java.io.IOException;
import java.util.*;
/** Synthetic camera-eye values; actor/real mission not loaded. */
public final class RoomCameraEyeWireTest {
    static void ok(boolean b){if(!b)throw new AssertionError("camera wire");}
    interface Action{void run()throws Exception;}
    static void bad(Action a)throws Exception{
        try{a.run();throw new AssertionError("Accepted invalid eye");}
        catch(IOException expected){}
    }
    public static void main(String[]args)throws Exception{
        byte[] session=new byte[16];Arrays.fill(session,(byte)11);
        var story=new RoomStoryWire.Ticket(session,99,12,0,0,7,15);
        byte[] frame=RoomCameraEyeWire.encode(story,1,-100,200,300);
        ok(frame.length==56&&frame[5]==RoomWire.HUMAN_CAMERA_EYE);
        var eye=RoomCameraEyeWire.parse(RoomWire.decode(frame));
        ok(eye.matches(story)&&eye.sequence==1&&eye.xCm==-100&&eye.yCm==200&&eye.zCm==300);
        session[0]=5;ok(eye.matches(story));
        bad(()->RoomCameraEyeWire.encode(story,0,1,2,3));
        bad(()->RoomCameraEyeWire.encode(story,1,20000001,0,0));
        bad(()->RoomCameraEyeWire.encode(story,1,0,-20000001,0));
        bad(()->RoomCameraEyeWire.encode(story,1,0,0,20000001));
        bad(()->RoomCameraEyeWire.parse(RoomWire.decode(RoomStoryWire.encode(RoomWire.STORY_PREPARE,story))));
        byte[] invalidEpoch=frame.clone();Arrays.fill(invalidEpoch,8+16+8+4,8+16+8+4+4,(byte)0);
        bad(()->RoomCameraEyeWire.parse(RoomWire.decode(invalidEpoch)));
        byte[] wrongVersion=frame.clone();wrongVersion[4]=5;
        bad(()->RoomWire.decode(wrongVersion));
        byte[] badTrailing=Arrays.copyOf(frame,frame.length+1);
        bad(()->RoomWire.decode(badTrailing));
        var different=new RoomStoryWire.Ticket(new byte[16],99,12,0,0,7,15);
        ok(!eye.matches(different));
        ok(!eye.matches(new RoomStoryWire.Ticket(story.session(),100,12,0,0,7,15)));
        ok(!eye.matches(new RoomStoryWire.Ticket(story.session(),99,13,0,0,7,15)));
        ok(!eye.matches(new RoomStoryWire.Ticket(story.session(),99,12,0,0,8,15)));
        System.out.println("CAMERA_EYE_WIRE_PASS: strict real-eye v6 schema, independent TLS identity and exact ticket");
        System.out.println("NO original body root, loaded story or gameplay actor produced");
    }
}
