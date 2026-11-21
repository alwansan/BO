// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import java.io.*;
import java.nio.*;
import java.nio.charset.*;
import java.security.MessageDigest;
import java.text.Normalizer;
import java.util.*;

/** Real room protocol only. It carries no fabricated actors or game snapshots. */
public final class RoomWire {
    private RoomWire() {}
    public static final int JOIN=1, ACCEPT=2, ROSTER=3, PING=4, PONG=5, LEAVE=6, REJECT=7;
    public static final int MAX_FRAME=512, MAX_PLAYERS=4, VERSION=1, PROFILE=1;
    private static final byte[] IMAGE=hex("c5bfed25449c8212d7a976f15246e87ead57713f");
    public static final class Incompatible extends IOException {
        private static final long serialVersionUID=1L;
        Incompatible(){super("Incompatible game image or room profile");}
    }
    public static final class Packet {
        public final int type;
        private final byte[] body;
        private Packet(int type,byte[] body){this.type=type;this.body=body;}
    }
    public static final class Join {
        public final String name, pin;
        public final byte[] nonce;
        public Join(String name,String pin,byte[] nonce){this.name=name;this.pin=pin;this.nonce=nonce.clone();}
    }
    public static final class Player {
        public final int id;
        public final String name;
        public Player(int id,String name){this.id=id;this.name=name;}
    }
    public static final class State {
        public final byte[] session, nonce;
        public final int self;
        public final long revision;
        public final String room;
        public final List<Player> players;
        private State(byte[] session,byte[] nonce,int self,long revision,String room,List<Player> players){
            this.session=session.clone();this.nonce=nonce.clone();this.self=self;this.revision=revision;
            this.room=room;this.players=Collections.unmodifiableList(new ArrayList<>(players));
        }
    }
    public static State copy(State value){
        return new State(value.session,value.nonce,value.self,value.revision,value.room,value.players);
    }
    static byte[] hex(String s){
        byte[] b=new byte[s.length()/2];
        for(int i=0;i<b.length;i++)b[i]=(byte)Integer.parseInt(s.substring(i*2,i*2+2),16);
        return b;
    }
    public static String name(String input) throws IOException {
        if(input==null)throw new IOException("Missing name");
        String s=Normalizer.normalize(input,Normalizer.Form.NFC);
        if(s.isEmpty()||!s.equals(s.trim()))throw new IOException("Empty or padded name");
        for(int i=0;i<s.length();){
            int cp=s.codePointAt(i);i+=Character.charCount(cp);
            int type=Character.getType(cp);
            if(Character.isISOControl(cp)||type==Character.FORMAT||type==Character.LINE_SEPARATOR||
               type==Character.PARAGRAPH_SEPARATOR||type==Character.SURROGATE)
                throw new IOException("Control character in name");
        }
        ByteBuffer encoded=StandardCharsets.UTF_8.newEncoder().onMalformedInput(CodingErrorAction.REPORT)
            .onUnmappableCharacter(CodingErrorAction.REPORT).encode(CharBuffer.wrap(s));
        if(encoded.remaining()>32)throw new IOException("Name exceeds 32 UTF-8 bytes");
        return s;
    }
    private static void putName(DataOutputStream out,String text)throws IOException {
        byte[] b=name(text).getBytes(StandardCharsets.UTF_8);out.writeByte(b.length);out.write(b);
    }
    private static String getName(DataInputStream in)throws IOException {
        int n=in.readUnsignedByte();if(n<1||n>32)throw new IOException("Bad name length");
        byte[] b=new byte[n];in.readFully(b);
        String raw=StandardCharsets.UTF_8.newDecoder().onMalformedInput(CodingErrorAction.REPORT)
            .onUnmappableCharacter(CodingErrorAction.REPORT).decode(ByteBuffer.wrap(b)).toString();
        String normalized=name(raw);if(!normalized.equals(raw))throw new IOException("Noncanonical name");
        return normalized;
    }
    private static void nonce(byte[] b)throws IOException{if(b==null||b.length!=16)throw new IOException("Bad nonce length");}
    private static void pin(String s)throws IOException {
        if(s==null||s.length()!=8)throw new IOException("PIN requires eight digits");
        for(int i=0;i<s.length();i++)if(s.charAt(i)<'0'||s.charAt(i)>'9')throw new IOException("Invalid PIN");
    }
    private static byte[] frame(int type,byte[] body)throws IOException {
        if(type<JOIN||type>REJECT||body.length>MAX_FRAME-8)throw new IOException("Invalid frame");
        ByteArrayOutputStream b=new ByteArrayOutputStream();DataOutputStream out=new DataOutputStream(b);
        out.writeInt(0x4243524d);out.writeByte(VERSION);out.writeByte(type);out.writeShort(body.length);out.write(body);
        return b.toByteArray();
    }
    public static Packet decode(byte[] raw)throws IOException {
        if(raw==null||raw.length<8||raw.length>MAX_FRAME)throw new IOException("Invalid frame size");
        DataInputStream in=new DataInputStream(new ByteArrayInputStream(raw));
        if(in.readInt()!=0x4243524d||in.readUnsignedByte()!=VERSION)throw new Incompatible();
        int type=in.readUnsignedByte(),n=in.readUnsignedShort();
        if(type<JOIN||type>REJECT||n!=raw.length-8)throw new IOException("Invalid type or length");
        byte[] body=new byte[n];in.readFully(body);return new Packet(type,body);
    }
    public static byte[] read(DataInputStream in)throws IOException {
        byte[] header=new byte[8];in.readFully(header);
        int n=((header[6]&255)<<8)|(header[7]&255);
        if(n>MAX_FRAME-8)throw new IOException("Oversize frame");
        byte[] raw=Arrays.copyOf(header,n+8);in.readFully(raw,8,n);decode(raw);return raw;
    }
    public static void write(DataOutputStream out,byte[] raw)throws IOException{decode(raw);out.write(raw);out.flush();}
    private static DataInputStream body(Packet p,int type)throws IOException{
        if(p.type!=type)throw new IOException("Unexpected message");
        return new DataInputStream(new ByteArrayInputStream(p.body));
    }
    private static void end(DataInputStream in)throws IOException{if(in.available()!=0)throw new IOException("Trailing data");}
    public static byte[] join(Join value)throws IOException {
        nonce(value.nonce);pin(value.pin);
        ByteArrayOutputStream b=new ByteArrayOutputStream();DataOutputStream out=new DataOutputStream(b);
        out.write(IMAGE);out.writeShort(PROFILE);out.write(value.nonce);putName(out,value.name);
        out.write(value.pin.getBytes(StandardCharsets.US_ASCII));return frame(JOIN,b.toByteArray());
    }
    public static Join parseJoin(Packet p)throws IOException {
        DataInputStream in=body(p,JOIN);byte[] image=new byte[20];in.readFully(image);
        if(!MessageDigest.isEqual(image,IMAGE)||in.readUnsignedShort()!=PROFILE)throw new Incompatible();
        byte[] nonce=new byte[16];in.readFully(nonce);String name=getName(in);
        byte[] digits=new byte[8];in.readFully(digits);String pin=new String(digits,StandardCharsets.US_ASCII);pin(pin);end(in);
        return new Join(name,pin,nonce);
    }
    private static void roster(int self,long revision,List<Player> players)throws IOException {
        if(self<1||revision<1||revision>0xffffffffL||players.isEmpty()||players.size()>MAX_PLAYERS)
            throw new IOException("Invalid roster");
        Set<Integer> ids=new HashSet<>();
        for(Player p:players)if(p.id<1||!ids.add(p.id))throw new IOException("Invalid player identity");
        if(!ids.contains(1)||!ids.contains(self))throw new IOException("Host or recipient missing");
    }
    public static byte[] state(int type,byte[] session,byte[] echo,int self,long revision,String room,List<Player> players)throws IOException {
        if(type!=ACCEPT&&type!=ROSTER)throw new IOException("Invalid state type");
        nonce(session);nonce(echo);roster(self,revision,players);
        ByteArrayOutputStream b=new ByteArrayOutputStream();DataOutputStream out=new DataOutputStream(b);
        out.write(session);out.write(echo);out.writeInt(self);out.writeInt((int)revision);putName(out,room);
        out.writeByte(MAX_PLAYERS);out.writeByte(players.size());
        for(Player p:players){out.writeInt(p.id);putName(out,p.name);}
        return frame(type,b.toByteArray());
    }
    public static State parseState(Packet p)throws IOException {
        if(p.type!=ACCEPT&&p.type!=ROSTER)throw new IOException("Unexpected state type");
        DataInputStream in=body(p,p.type);byte[] session=new byte[16],echo=new byte[16];in.readFully(session);in.readFully(echo);
        int self=in.readInt();long revision=in.readInt()&0xffffffffL;String room=getName(in);
        int capacity=in.readUnsignedByte(),n=in.readUnsignedByte();
        if(capacity!=MAX_PLAYERS||n<1||n>capacity)throw new IOException("Invalid capacity");
        List<Player> players=new ArrayList<>();for(int i=0;i<n;i++)players.add(new Player(in.readInt(),getName(in)));
        end(in);roster(self,revision,players);return new State(session,echo,self,revision,room,players);
    }
    public static byte[] pulse(int type,long sequence)throws IOException {
        if((type!=PING&&type!=PONG)||sequence<1)throw new IOException("Invalid pulse");
        ByteArrayOutputStream b=new ByteArrayOutputStream();DataOutputStream out=new DataOutputStream(b);out.writeLong(sequence);
        return frame(type,b.toByteArray());
    }
    public static long parsePulse(Packet p)throws IOException {
        if(p.type!=PING&&p.type!=PONG)throw new IOException("Unexpected pulse");
        DataInputStream in=body(p,p.type);long s=in.readLong();end(in);if(s<1)throw new IOException("Bad sequence");return s;
    }
    public static byte[] leave()throws IOException{return frame(LEAVE,new byte[0]);}
    public static void parseLeave(Packet p)throws IOException{end(body(p,LEAVE));}
    public static byte[] reject(int reason)throws IOException {
        if(reason<1||reason>5)throw new IOException("Invalid rejection");return frame(REJECT,new byte[]{(byte)reason});
    }
    public static int parseReject(Packet p)throws IOException{
        DataInputStream in=body(p,REJECT);int r=in.readUnsignedByte();end(in);if(r<1||r>5)throw new IOException("Bad rejection");return r;
    }
}
