// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.InputFilter;
import android.text.InputType;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;

/**
 * Opt-in Android host/join form. Standalone fixture until connected to the
 * game's Flash menu. Never claims that an authenticated peer is a game actor.
 */
public final class RoomLobbyActivity extends Activity {
    private final Handler ui=new Handler(Looper.getMainLooper());
    private RoomSession session;
    private LinearLayout content;
    private EditText player,room,pin,listenPort,target,fingerprint;
    private TextView status;
    private String lastText="";
    private final Runnable poll=new Runnable(){
        @Override public void run(){
            if(session==null)return;
            RoomSession.Status current=session.snapshot();
            String next=render(current);
            if(!next.equals(lastText)){status.setText(next);lastText=next;}
            ui.postDelayed(this,350L);
        }
    };
    private TextView caption(String value){
        TextView text=new TextView(this);
        text.setText(value);text.setTextSize(15f);text.setPadding(0,12,0,4);
        content.addView(text);return text;
    }
    private EditText field(String label,String hint,int inputType,int maxLength) {
        caption(label);
        EditText edit=new EditText(this);
        edit.setSingleLine(true);
        edit.setInputType(inputType);
        edit.setHint(hint);
        edit.setFilters(new InputFilter[]{new InputFilter.LengthFilter(maxLength)});
        content.addView(edit);
        return edit;
    }
    private void button(String label,View.OnClickListener callback){
        Button button=new Button(this);button.setText(label);
        button.setOnClickListener(callback);content.addView(button);
    }
    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        session=RoomSession.create();
        ScrollView scroll=new ScrollView(this);
        content=new LinearLayout(this);
        content.setOrientation(LinearLayout.VERTICAL);
        int p=(int)(18*getResources().getDisplayMetrics().density);
        content.setPadding(p,p,p,p);
        scroll.addView(content);setContentView(scroll);
        setTitle("BLACK CO-OP — Room connection test");
        caption("Connection-only test • up to 4 real humans, including phone host. "
            +"No second game character, combat or story synchronization yet. "
            +"Use a trusted LAN/VPN. Do not expose the port to the public internet.");
        player=field("Your display name","Player",InputType.TYPE_CLASS_TEXT,32);
        room=field("Host room name","BLACK Room",InputType.TYPE_CLASS_TEXT,32);
        pin=field("Eight-digit room PIN (private)","8 digits",
            InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_VARIATION_PASSWORD,8);
        listenPort=field("Host listening port","24872",InputType.TYPE_CLASS_NUMBER,5);
        listenPort.setText("24872");
        button("HOST — create secure room",v->startHost());
        target=field("JOIN target (canonical IPv4:port)","192.168.1.10:24872",
            InputType.TYPE_CLASS_TEXT,21);
        fingerprint=field("Host TLS fingerprint from trusted channel (64 hex digits)",
            "SHA-256 certificate fingerprint",InputType.TYPE_CLASS_TEXT,95);
        button("JOIN — verify host and PIN",v->join());
        button("LEAVE / CANCEL",v->{session.stop();display("Disconnected");});
        caption("Real roster / connection status");
        status=new TextView(this);status.setTextIsSelectable(true);
        status.setTextSize(15f);status.setPadding(0,12,0,24);
        content.addView(status);
        poll.run();
    }
    private void display(String text) {lastText=text;status.setText(text);}
    private String value(EditText edit){return edit.getText().toString();}
    private void startHost() {
        final String name=value(player),title=value(room),secret=value(pin);
        final int port;
        try{
            port=Integer.parseInt(value(listenPort));
            if(port<1024||port>65535||secret.length()!=8)
                throw new IllegalArgumentException();
            RoomWire.name(name);RoomWire.name(title);
            if(!secret.matches("[0-9]{8}"))throw new IllegalArgumentException();
        }catch(Exception invalid){display("Enter a name, room title, 8-digit PIN and port 1024-65535");return;}
        try{
            session.startHost(()->{
                RoomIdentity identity=RoomIdentity.load();
                return new RoomSession.HostIdentity(identity.serverContext(),identity.fingerprint());
            },InetAddress.getByAddress(new byte[]{0,0,0,0}),port,name,title,secret);
        }catch(Exception error){display("Could not start room");}
    }
    private void join(){
        final String name=value(player),secret=value(pin);
        final String address=value(target),hash=value(fingerprint);
        try{
            RoomWire.name(name);
            RoomDirect.parse(address);
            RoomDirect.fingerprint(hash);
            if(!secret.matches("[0-9]{8}"))throw new IllegalArgumentException();
            session.join(address,hash,name,secret);
        }catch(Exception invalid){
            display("Enter a valid name, IPv4:port, full TLS fingerprint and 8-digit PIN");
        }
    }
    private static String addresses(){
        StringBuilder result=new StringBuilder();
        try{
            Enumeration<NetworkInterface> items=NetworkInterface.getNetworkInterfaces();
            int shown=0;
            while(items!=null&&items.hasMoreElements()&&shown<6){
                NetworkInterface network=items.nextElement();
                if(!network.isUp()||network.isLoopback())continue;
                Enumeration<InetAddress> addrs=network.getInetAddresses();
                while(addrs.hasMoreElements()&&shown<6){
                    InetAddress address=addrs.nextElement();
                    if(address instanceof Inet4Address&&!address.isLoopbackAddress()&&
                       !address.isAnyLocalAddress()&&!address.isLinkLocalAddress()){
                        result.append(address.getHostAddress()).append(" (")
                              .append(network.getName()).append(")\n");
                        shown++;
                    }
                }
            }
        }catch(Exception ignored){}
        return result.toString();
    }
    private static String render(RoomSession.Status current){
        StringBuilder result=new StringBuilder();
        result.append("State: ").append(current.phase).append("\n");
        if(!current.description.isEmpty())result.append(current.description).append("\n");
        if(current.phase==RoomSession.Phase.HOSTING){
            result.append("Port: ").append(current.port).append("\n");
            result.append("Local IPv4 addresses (select reachable LAN/VPN):\n")
                  .append(addresses());
            result.append("Certificate SHA-256 fingerprint:\n")
                  .append(current.fingerprint).append("\n");
            result.append("Share address + fingerprint by a trusted channel; "
                         +"share PIN privately. A LAN discovery reply is not authentication.\n");
        }
        if(current.roster!=null){
            result.append("Actual connected humans: ")
                  .append(current.roster.players.size()).append("/4\n");
            for(RoomWire.Player p:current.roster.players){
                result.append(p.id==current.roster.self?"* ":"  ")
                      .append(p.id).append(" — ").append(p.name).append("\n");
            }
        }
        return result.toString();
    }
    @Override protected void onStop(){
        ui.removeCallbacks(poll);
        if(session!=null)session.stop();
        super.onStop();
    }
    @Override protected void onStart(){
        super.onStart();
        if(session!=null){ui.removeCallbacks(poll);ui.post(poll);}
    }
    @Override protected void onDestroy(){
        ui.removeCallbacks(poll);
        if(session!=null){session.close();session=null;}
        super.onDestroy();
    }
}
