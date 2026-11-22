package com.racas.black.coop;
import java.io.IOException;
import java.util.*;
/** Synthetic actor positions ONLY. BLACK mission/actors are not loaded here. */
public final class RoomWorldSnapshotWireTest {
    static void ok(boolean b){if(!b)throw new AssertionError("World snapshot");}
    interface Fault{void run()throws Exception;}
    static void invalid(Fault r)throws Exception{
        try{r.run();throw new AssertionError("Accepted bad snapshot");}
        catch(IOException expected){}
    }
    static RoomWorldSnapshotWire.Avatar A(int id,int x)throws IOException{
        return new RoomWorldSnapshotWire.Avatar(id,x,0,100,9000,0);
    }
    public static void main(String[]args)throws Exception{
        byte[] session=new byte[16];Arrays.fill(session,(byte)22);
        RoomStoryWire.Ticket ticket=new RoomStoryWire.Ticket(session,100,7,0,0,3,15);
        List<RoomWorldSnapshotWire.Avatar> positions=new ArrayList<>(Arrays.asList(A(1,0),A(2,3),A(3,6),A(4,9)));
        RoomWorldSnapshotWire.Snapshot source=RoomWorldSnapshotWire.create(ticket,1,positions);
        byte[] data=RoomWorldSnapshotWire.encode(source);
        ok(data.length==121&&data[5]==RoomWire.WORLD_SNAPSHOT);
        RoomWorldSnapshotWire.Snapshot decoded=
            RoomWorldSnapshotWire.parse(RoomWire.decode(data));
        ok(decoded.matches(ticket)&&decoded.tick==1&&decoded.avatars.size()==4);
        ok(decoded.avatars.get(3).playerId==4&&decoded.avatars.get(3).xCm==9);
        positions.clear(); // immutable-by-copy snapshot must survive source list reuse
        invalid(()->RoomWorldSnapshotWire.create(ticket,0,Arrays.asList(A(1,1))));
        invalid(()->RoomWorldSnapshotWire.create(ticket,2,Arrays.asList(A(2,1))));
        invalid(()->RoomWorldSnapshotWire.create(ticket,2,Arrays.asList(A(1,0),A(1,1))));
        invalid(()->RoomWorldSnapshotWire.create(ticket,2,Arrays.asList(A(1,0),A(3,1),A(2,1))));
        invalid(()->RoomWorldSnapshotWire.create(ticket,2,Collections.emptyList()));
        invalid(()->new RoomWorldSnapshotWire.Avatar(1,20000001,0,0,0,0));
        invalid(()->new RoomWorldSnapshotWire.Avatar(1,0,0,0,18001,0));
        invalid(()->new RoomWorldSnapshotWire.Avatar(1,0,0,0,0,4));
        byte[] invalidVersion=data.clone();invalidVersion[4]=4;
        invalid(()->RoomWire.decode(invalidVersion));
        byte[] invalidCount=data.clone();invalidCount[8+16+8+4+4+4]=5;
        invalid(()->RoomWorldSnapshotWire.parse(RoomWire.decode(invalidCount)));
        byte[] invalidPlayer=data.clone();invalidPlayer[8+37+3]=0;
        invalid(()->RoomWorldSnapshotWire.parse(RoomWire.decode(invalidPlayer)));
        byte[] invalidLength=Arrays.copyOf(data,data.length+1);
        invalid(()->RoomWire.decode(invalidLength));
        var wrong=new RoomStoryWire.Ticket(new byte[16],100,7,0,0,3,15);
        ok(!decoded.matches(wrong));
        ok(!decoded.matches(new RoomStoryWire.Ticket(ticket.session(),101,7,0,0,3,15)));
        ok(!decoded.matches(new RoomStoryWire.Ticket(ticket.session(),100,8,0,0,3,15)));
        ok(!decoded.matches(new RoomStoryWire.Ticket(ticket.session(),100,7,0,0,4,15)));
        System.out.println("WORLD_SNAPSHOT_WIRE_PASS: strict host-only 4-person state frame, immutable validated bounded positions, epoch and roster pin");
        System.out.println("NO ORIGINAL BLACK actor positions sampled; synthetic only");
    }
}
