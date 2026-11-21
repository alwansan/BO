// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.net.InetAddress;
import java.nio.file.Paths;
import javax.net.ssl.SSLContext;

/** Real JVM socket test of asynchronous host/join ownership and stale requests. */
public final class SessionTest {
    private interface Check { boolean valid() throws Exception; }
    private static void check(boolean okay){if(!okay)throw new AssertionError("RoomSession invariant");}
    private static void until(Check assertion)throws Exception {
        long end=System.nanoTime()+8_000_000_000L;
        while(!assertion.valid()){
            if(System.nanoTime()>end)throw new AssertionError("Timed out waiting for room lifecycle");
            Thread.sleep(20);
        }
    }
    public static void main(String[]args)throws Exception{
        SSLContext server=TlsRoomTest.host(Paths.get(args[0]));
        byte[] pin=TlsRoomTest.fingerprint(Paths.get(args[0]));
        RoomSession.IdentitySource source=()->new RoomSession.HostIdentity(server,pin);
        InetAddress loop=InetAddress.getLoopbackAddress();
        try(RoomSession hosting=RoomSession.create();RoomSession joining=RoomSession.create()){
            hosting.startHost(source,loop,0,"Phone host","BLACK coop","01234567");
            until(()->hosting.snapshot().phase==RoomSession.Phase.HOSTING);
            RoomSession.Status start=hosting.snapshot();
            check(start.port>0&&start.fingerprint.length()==64&&start.roster.players.size()==1);
            String address="127.0.0.1:"+start.port;
            joining.join(address,start.fingerprint,"Phone client","01234567");
            until(()->joining.snapshot().phase==RoomSession.Phase.JOINED);
            until(()->hosting.snapshot().roster.players.size()==2&&joining.snapshot().roster.players.size()==2);
            byte[] stolen=joining.snapshot().roster.session;
            stolen[0]^=42;
            check(joining.snapshot().roster.session[0]!=stolen[0]);
            joining.stop();
            until(()->hosting.snapshot().roster.players.size()==1);
            joining.join(address,start.fingerprint,"Bad PIN","99999999");
            until(()->joining.snapshot().phase==RoomSession.Phase.ERROR);
            check(joining.snapshot().description.equals("Wrong room PIN"));
            // Rejected stale join may finish later, but cannot override a newer stop.
            joining.join(address,start.fingerprint,"Cancelled","01234567");
            joining.stop();
            check(joining.snapshot().phase==RoomSession.Phase.IDLE);
            hosting.stop();
            check(hosting.snapshot().phase==RoomSession.Phase.IDLE);
            joining.join(address,start.fingerprint,"After shutdown","01234567");
            until(()->joining.snapshot().phase==RoomSession.Phase.ERROR);
        }
        System.out.println("ROOM_SESSION_PASS: real host/join, immutable roster, reject, cancellation, lifecycle");
    }
}
