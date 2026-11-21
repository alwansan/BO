// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import android.security.keystore.KeyGenParameterSpec;
import android.security.keystore.KeyProperties;
import java.net.Socket;
import java.security.*;
import java.security.cert.X509Certificate;
import java.util.Date;
import javax.net.ssl.*;
import javax.security.auth.x500.X500Principal;

/** Per-install opaque AndroidKeyStore key. No embedded/exported server private key. */
public final class RoomIdentity {
    private static final String ALIAS="BLACK_COOP_TLS_ID_V1";
    private final SSLContext server;
    private final byte[] fingerprint;
    private RoomIdentity(SSLContext server,byte[] fingerprint){this.server=server;this.fingerprint=fingerprint.clone();}
    public SSLContext serverContext(){return server;}
    public byte[] fingerprint(){return fingerprint.clone();}
    public String fingerprintHex(){
        StringBuilder s=new StringBuilder(64);for(byte b:fingerprint)s.append(String.format(java.util.Locale.ROOT,"%02X",b&255));return s.toString();
    }
    public static synchronized RoomIdentity load()throws GeneralSecurityException,java.io.IOException{
        KeyStore store=KeyStore.getInstance("AndroidKeyStore");store.load(null);
        if(!store.containsAlias(ALIAS)){
            long time=System.currentTimeMillis();
            KeyPairGenerator generator=KeyPairGenerator.getInstance(KeyProperties.KEY_ALGORITHM_RSA,"AndroidKeyStore");
            generator.initialize(new KeyGenParameterSpec.Builder(ALIAS,KeyProperties.PURPOSE_SIGN|KeyProperties.PURPOSE_VERIFY)
                .setKeySize(2048)
                // TLS Conscrypt hashes the handshake transcript itself and may ask
                // AndroidKeyStore to sign a PREHASHED value. Official
                // KeyGenParameterSpec docs require DIGEST_NONE for this.
                .setDigests(KeyProperties.DIGEST_NONE,KeyProperties.DIGEST_SHA256,
                            KeyProperties.DIGEST_SHA384,KeyProperties.DIGEST_SHA512)
                .setSignaturePaddings(KeyProperties.SIGNATURE_PADDING_RSA_PKCS1,KeyProperties.SIGNATURE_PADDING_RSA_PSS)
                .setCertificateSubject(new X500Principal("CN=BLACK Co-op Room"))
                .setCertificateSerialNumber(new java.math.BigInteger(128,new SecureRandom()).setBit(127))
                .setCertificateNotBefore(new Date(time-86400000L))
                .setCertificateNotAfter(new Date(time+3650L*86400000L))
                .build());
            generator.generateKeyPair();
        }
        PrivateKey key=(PrivateKey)store.getKey(ALIAS,null);
        java.security.cert.Certificate[] stored=store.getCertificateChain(ALIAS);
        if(key==null||key.getEncoded()!=null||stored==null||stored.length!=1||!(stored[0] instanceof X509Certificate))
            throw new KeyStoreException("Missing or unexpected opaque room identity");
        X509Certificate certificate=(X509Certificate)stored[0];certificate.checkValidity();
        X509ExtendedKeyManager manager=new X509ExtendedKeyManager(){
            private boolean rsa(String type){return "RSA".equals(type);}
            public String[] getClientAliases(String type,Principal[] issuers){return null;}
            public String chooseClientAlias(String[] types,Principal[] issuers,Socket socket){return null;}
            public String[] getServerAliases(String type,Principal[] issuers){return rsa(type)?new String[]{ALIAS}:null;}
            public String chooseServerAlias(String type,Principal[] issuers,Socket socket){return rsa(type)?ALIAS:null;}
            public String chooseEngineServerAlias(String type,Principal[] issuers,SSLEngine engine){return rsa(type)?ALIAS:null;}
            public X509Certificate[] getCertificateChain(String alias){return ALIAS.equals(alias)?new X509Certificate[]{certificate}:null;}
            public PrivateKey getPrivateKey(String alias){return ALIAS.equals(alias)?key:null;}
        };
        SSLContext server=SSLContext.getInstance("TLS");server.init(new KeyManager[]{manager},null,new SecureRandom());
        return new RoomIdentity(server,MessageDigest.getInstance("SHA-256").digest(certificate.getEncoded()));
    }
}
