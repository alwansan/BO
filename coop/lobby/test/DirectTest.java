package com.racas.black.coop;
import java.io.IOException;
import java.net.InetAddress;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Locale;
import java.util.concurrent.TimeUnit;

public final class DirectTest {
    private DirectTest() {}
    private static void check(boolean ok) {
        if(!ok)throw new AssertionError("Direct JOIN invariant failed");
    }
    private interface Task {void run()throws Exception;}
    private static void rejected(Task action)throws Exception {
        try{action.run();throw new AssertionError("Expected strict validation rejection");}
        catch(IOException expected){}
    }
    private static String toHex(byte[] bytes) {
        StringBuilder result=new StringBuilder();
        for(byte b:bytes)result.append(String.format(Locale.ROOT,"%02X",b&255));
        return result.toString();
    }
    public static void main(String[] args)throws Exception {
        RoomDirect.Endpoint target=RoomDirect.parse("192.168.1.9:1508");
        check(target.port==1508&&target.address.getHostAddress().equals("192.168.1.9"));
        for(String invalid:new String[]{"","localhost:1508","192.168.1.1","192.168.1.1:0",
                "192.168.1.1:80","192.168.1.1:65536","192.168.1.1:1a",
                "192.168.001.1:1508","192.168.1.1:01508","192.168.1:1508",
                "0.0.0.0:1508","255.255.255.255:1508","239.1.2.3:1508",
                "192.168.1.1:1508 ","192.168.1.1:1508\n","[::1]:1508",
                "192.168.1.1:1508:1510","192.168.1..1:1508"}) {
            rejected(()->RoomDirect.parse(invalid));
        }
        byte[] pin=TlsRoomTest.fingerprint(Paths.get(args[0]));
        String canonical=toHex(pin);
        check(java.util.Arrays.equals(pin,RoomDirect.fingerprint(canonical)));
        check(java.util.Arrays.equals(pin,RoomDirect.fingerprint(canonical.toLowerCase(Locale.ROOT))));
        StringBuilder colon=new StringBuilder();
        for(int i=0;i<32;i++) {
            if(i>0)colon.append(':');
            colon.append(canonical,i*2,i*2+2);
        }
        check(java.util.Arrays.equals(pin,RoomDirect.fingerprint(colon.toString())));
        for(String invalid:new String[]{"",canonical+"0",canonical.substring(1),
                canonical.substring(0,63)+"Z",colon.toString().replace(':','-'),
                canonical.substring(0,63)+"\u0661"}) {
            rejected(()->RoomDirect.fingerprint(invalid));
        }
        final InetAddress loop=InetAddress.getByName("127.0.0.1");
        try(RoomTls.Server host=RoomTls.Server.start(TlsRoomTest.host(Paths.get(args[0])),
                 loop,0,"Host","Direct room","12345678")) {
            String endpoint="127.0.0.1:"+host.port();
            try(RoomTls.Client peer=RoomDirect.join(endpoint,canonical,"Player2","12345678")) {
                TlsRoomTest.await(()->host.snapshot().players.size()==2);
                check(peer.snapshot()!=null&&peer.snapshot().self==2);
                check(peer.isRunning());
            }
            TlsRoomTest.await(()->host.snapshot().players.size()==1);
            byte[] wrong=pin.clone();wrong[31]^=1;
            rejected(()->{try(RoomTls.Client unused=RoomDirect.join(endpoint,toHex(wrong),
                                           "Wrong fingerprint","12345678")){check(unused.isRunning());}});
            rejected(()->{try(RoomTls.Client unused=RoomDirect.join(endpoint,canonical,
                                           "Wrong PIN","99999999")){check(unused.isRunning());}});
            check(host.snapshot().players.size()==1);
        }
        TlsRoomTest.await(()->Thread.getAllStackTraces().keySet().stream()
            .noneMatch(t->t.isAlive()&&t.getName().startsWith("bo-room-")));
        System.out.println("PASS direct IPv4:port input and 32-byte pin; real TLS JOIN, reject spoof/pin/PIN, cleanup");
        System.out.println("No Android forms, gameplay actors or network-thread guest calls");
    }
}
