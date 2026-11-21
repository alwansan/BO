// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.security.GeneralSecurityException;

/** Strict, DNS-free direct IPv4 joining with a full out-of-band TLS pin.
 *  This does not connect to BLACK gameplay or expose the server identity
 *  through untrusted LAN discovery. The pin must come from a trusted
 *  separate channel (e.g., the host's fingerprint display/QR).
 */
public final class RoomDirect {
    private RoomDirect() {}
    public static final class Endpoint {
        public final Inet4Address address;
        public final int port;
        private Endpoint(Inet4Address address,int port) {
            this.address=address;this.port=port;
        }
    }
    private static int decimal(String s,int max) throws IOException {
        if(s.isEmpty() || s.length()>5 || (s.length()>1&&s.charAt(0)=='0'))
            throw new IOException("Noncanonical numeric address");
        int value=0;
        for(int i=0;i<s.length();i++) {
            char c=s.charAt(i);
            if(c<'0'||c>'9')throw new IOException("Invalid decimal component");
            value=value*10+(c-'0');
            if(value>max)throw new IOException("Numeric value out of range");
        }
        return value;
    }
    /** The UI must not resolve hostnames implicitly or accept broadcast/multicast. */
    public static Endpoint parse(String target) throws IOException {
        if(target==null||target.isEmpty()||target.length()>21||
           !target.equals(target.trim()))throw new IOException("Invalid direct address");
        final int colon=target.indexOf(':');
        if(colon<1||colon!=target.lastIndexOf(':'))
            throw new IOException("Explicit IPv4:port is required");
        String[] octets=target.substring(0,colon).split("\\.",-1);
        if(octets.length!=4)throw new IOException("Four IPv4 octets required");
        byte[] raw=new byte[4];
        for(int i=0;i<4;i++)raw[i]=(byte)decimal(octets[i],255);
        int port=decimal(target.substring(colon+1),65535);
        if(port<1024)throw new IOException("Unprivileged game port required");
        int first=raw[0]&255;
        if(first==0||first>=224||
           (first==255 && (raw[1]&255)==255))
            throw new IOException("Invalid destination IPv4");
        // getByAddress does not query DNS, and cannot choose IPv6.
        InetAddress address=InetAddress.getByAddress(raw);
        if(!(address instanceof Inet4Address) ||
           address.isAnyLocalAddress() || address.isMulticastAddress())
            throw new IOException("Invalid destination type");
        return new Endpoint((Inet4Address)address,port);
    }
    /** Accept 64 hex characters, or 32 two-digit hex pairs separated by ':'. */
    public static byte[] fingerprint(String text) throws IOException {
        if(text==null)throw new IOException("Full SHA-256 pin required");
        String normalized;
        if(text.length()==64) {
            normalized=text;
        } else if(text.length()==95) {
            StringBuilder builder=new StringBuilder(64);
            for(int i=0;i<32;i++) {
                int pos=i*3;
                if(i!=31&&text.charAt(pos+2)!=':')
                    throw new IOException("Bad certificate fingerprint format");
                builder.append(text,pos,pos+2);
            }
            normalized=builder.toString();
        } else {
            throw new IOException("Full SHA-256 pin required");
        }
        byte[] pin=new byte[32];
        for(int i=0;i<32;i++) {
            int hi=Character.digit(normalized.charAt(i*2),16);
            int lo=Character.digit(normalized.charAt(i*2+1),16);
            if(hi<0||lo<0||normalized.charAt(i*2)>127||
               normalized.charAt(i*2+1)>127)
                throw new IOException("Invalid certificate fingerprint");
            pin[i]=(byte)((hi<<4)|lo);
        }
        return pin;
    }
    /** Real TLS and JOIN; errors are fail-closed and never expose the PIN. */
    public static RoomTls.Client join(String ipv4Port,String hexFingerprint,
                                      String displayName,String roomPin)
            throws IOException,GeneralSecurityException {
        Endpoint target=parse(ipv4Port);
        byte[] fullPin=fingerprint(hexFingerprint);
        return RoomTls.Client.connect(RoomTls.pinnedContext(fullPin),
                                      target.address,target.port,displayName,roomPin);
    }
}
