package com.racas.black.coop;
import java.io.IOException;
import java.util.Arrays;
/** Pure wire tests: NOT actual BLACK character motion. */
public final class RoomHumanInputWireTest {
    static void ok(boolean good){if(!good)throw new AssertionError("Human input invariant");}
    interface Test{void run()throws Exception;}
    static void bad(Test t)throws Exception{
        try{t.run();throw new AssertionError("Malformed input accepted");}
        catch(IOException expected){}
    }
    public static void main(String[]args)throws Exception{
        byte[] session=new byte[16];Arrays.fill(session,(byte)73);
        RoomStoryWire.Ticket ticket=new RoomStoryWire.Ticket(session,99,5,0,0,7,3);
        byte[] frame=RoomHumanInputWire.encode(ticket,3,-1000,1000,-18000,true,false);
        ok(frame.length==51&&frame[5]==RoomWire.HUMAN_INPUT);
        var command=RoomHumanInputWire.parse(RoomWire.decode(frame));
        ok(command.matches(ticket));
        ok(command.sequence==3&&command.moveX==-1000&&command.moveY==1000);
        ok(command.yaw==-18000&&command.fire&&!command.jump);
        session[0]=1;ok(command.matches(ticket));
        bad(()->RoomHumanInputWire.encode(ticket,0,0,0,0,false,false));
        bad(()->RoomHumanInputWire.encode(ticket,1,1001,0,0,false,false));
        bad(()->RoomHumanInputWire.encode(ticket,1,0,-1001,0,false,false));
        bad(()->RoomHumanInputWire.encode(ticket,1,0,0,18001,false,false));
        bad(()->RoomHumanInputWire.parse(RoomWire.decode(RoomStoryWire.encode(RoomWire.STORY_READY,ticket))));
        byte[] invalidFlags=frame.clone();invalidFlags[invalidFlags.length-1]=4;
        bad(()->RoomHumanInputWire.parse(RoomWire.decode(invalidFlags)));
        byte[] invalidEpoch=frame.clone();
        Arrays.fill(invalidEpoch,8+16+8+4,8+16+8+4+4,(byte)0);
        bad(()->RoomHumanInputWire.parse(RoomWire.decode(invalidEpoch)));
        byte[] wrong=frame.clone();wrong[4]=3;
        bad(()->RoomWire.decode(wrong));
        var different=new RoomStoryWire.Ticket(new byte[16],99,5,0,0,7,3);
        ok(!command.matches(different));
        ok(!command.matches(new RoomStoryWire.Ticket(ticket.session(),100,5,0,0,7,3)));
        ok(!command.matches(new RoomStoryWire.Ticket(ticket.session(),99,5,0,0,8,3)));
        ok(!command.matches(new RoomStoryWire.Ticket(ticket.session(),99,6,0,0,7,3)));
        System.out.println("HUMAN_INPUT_WIRE_PASS: version4 strict bounded TLS frame and story-session/epoch pin");
        System.out.println("NO player ID can be spoofed in payload; server TLS must supply it");
    }
}
