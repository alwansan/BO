// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
import java.io.*;
import java.net.*;
import java.security.*;
import java.security.cert.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import javax.net.ssl.*;

/** Acquire the current peer certificate for automatic LAN pairing; never sends application data.
 * This is opportunistic encryption, not out-of-band authenticated peer identity. */
public final class RoomPeerTrust {
    private RoomPeerTrust() {}
    private static void close(Socket socket){try{socket.close();}catch(IOException ignored){}}
    public static byte[] inspect(InetAddress address,int port)throws IOException{
        return inspect(address,port,()->false);
    }
    static byte[] inspect(InetAddress address,int port,java.util.function.BooleanSupplier cancelled)throws IOException{
        if(cancelled.getAsBoolean())throw new IOException("Join cancelled");
        if(address==null||port<1024||port>65535)throw new IOException("Invalid peer endpoint");
        AtomicReference<byte[]> observed=new AtomicReference<>();
        X509TrustManager reject=new X509TrustManager(){
            public X509Certificate[] getAcceptedIssuers(){return new X509Certificate[0];}
            public void checkClientTrusted(X509Certificate[] chain,String auth)throws CertificateException{throw new CertificateException("Not trusted");}
            public void checkServerTrusted(X509Certificate[] chain,String auth)throws CertificateException{
                if(chain==null||chain.length<1||chain.length>8)throw new CertificateException("Bad identity");
                chain[0].checkValidity();
                try{observed.set(MessageDigest.getInstance("SHA-256").digest(chain[0].getEncoded()));}
                catch(NoSuchAlgorithmException e){throw new CertificateException(e);}
                // Deliberately abort before application data. The caller pins this observed identity on a fresh connection.
                throw new CertificateException("Identity inspection complete");
            }
        };
        final SSLContext context;
        try{context=SSLContext.getInstance("TLS");context.init(null,new TrustManager[]{reject},new SecureRandom());}
        catch(GeneralSecurityException e){throw new IOException("TLS identity inspection unavailable",e);}
        try(SSLSocket socket=(SSLSocket)context.getSocketFactory().createSocket()){
            List<String> allowed=new ArrayList<>();
            for(String s:socket.getSupportedProtocols())if(s.equals("TLSv1.3")||s.equals("TLSv1.2"))allowed.add(s);
            if(allowed.isEmpty())throw new IOException("TLS 1.2 or later required");
            socket.setEnabledProtocols(allowed.toArray(new String[0]));socket.setSoTimeout(2000);
            ScheduledExecutorService deadline=Executors.newSingleThreadScheduledExecutor(work->{Thread t=new Thread(work,"bo-room-probe-deadline");t.setDaemon(true);return t;});
            try{
                deadline.schedule(()->close(socket),5,TimeUnit.SECONDS);
                deadline.scheduleAtFixedRate(()->{if(cancelled.getAsBoolean())close(socket);},0,50,TimeUnit.MILLISECONDS);
                socket.connect(new InetSocketAddress(address,port),2000);
                try{socket.startHandshake();}
                catch(IOException expectedRejection){byte[] value=observed.get();if(value!=null)return value.clone();throw expectedRejection;}
                throw new IOException("Identity inspection unexpectedly accepted TLS");
            }finally{deadline.shutdownNow();}
        }
    }
    public static String hex(byte[] fingerprint){
        if(fingerprint==null||fingerprint.length!=32)throw new IllegalArgumentException("Full fingerprint required");
        StringBuilder s=new StringBuilder(64);for(byte b:fingerprint)s.append(String.format(java.util.Locale.ROOT,"%02X",b&255));return s.toString();
    }
}
