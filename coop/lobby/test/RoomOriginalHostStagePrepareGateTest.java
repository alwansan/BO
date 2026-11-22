package com.racas.black.coop;
public final class RoomOriginalHostStagePrepareGateTest {
    static void ok(boolean yes){if(!yes)throw new AssertionError("source stage host PREPARE");}
    public static void main(String[]args) {
        RoomOriginalHostStagePrepareGate gate=new RoomOriginalHostStagePrepareGate();
        ok(gate.nextOffer(1,1,false,10)==0);
        ok(!gate.observe(0,0,0,10));
        ok(gate.observe(1,0,0,100));
        ok(!gate.observe(1,0,1,150));
        ok(gate.nextOffer(1,0,false,160)==0);
        ok(gate.nextOffer(1,1,true,160)==0);
        ok(gate.nextOffer(1,1,false,160)==1);
        ok(gate.markOffered(1,1));
        ok(gate.nextOffer(1,1,false,180)==0);
        ok(gate.nextOffer(2,2,false,200)==2);
        ok(gate.markOffered(2,2));
        ok(gate.nextOffer(2,2,false,220)==0);
        ok(gate.nextOffer(3,1,false,60000000400L)==0);
        ok(!gate.observe(1,0,0,60000000410L));
        ok(gate.nextOffer(3,1,false,60000000420L)==3);
        ok(gate.markOffered(3,3));
        ok(gate.observe(2,8,0,60000000430L));
        ok(gate.nextOffer(4,1,false,60000000440L)==0);
        ok(gate.observe(3,7,15,60000000450L));
        ok(gate.nextOffer(5,3,false,60000000460L)==4);
        ok(!gate.observe(2,7,15,60000000470L));
        ok(gate.level()==7&&gate.stage()==15);
        gate.reset();
        ok(gate.nextOffer(5,3,false,60000000480L)==0);
        ok(gate.observe(9,3,0,60000000490L));
        ok(gate.nextOffer(5,1,false,60000000500L)==1);
        System.out.println("ORIGINAL_HOST_STAGE_PREPARE_GATE_PASS: late join, replay, roster, source TTL; no READY/COMMIT");
    }
}