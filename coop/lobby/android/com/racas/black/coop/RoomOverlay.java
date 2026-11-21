// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.GradientDrawable;
import android.os.Handler;
import android.os.Looper;
import android.text.InputFilter;
import android.text.InputType;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import java.net.InetAddress;
import java.net.Inet4Address;
import java.net.NetworkInterface;
import java.util.Enumeration;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

/** In-process, game-styled lobby. Android Back dismisses this Dialog, never the game. */
public final class RoomOverlay {
    private static RoomOverlay active;
    private static final int FG=Color.rgb(226,223,213), MUTED=Color.rgb(170,167,162);
    private final Activity activity;
    private final RoomSession session;
    private final Dialog dialog;
    private final Handler handler=new Handler(Looper.getMainLooper());
    private final ExecutorService worker=Executors.newSingleThreadExecutor(r->{
        Thread t=new Thread(r,"black-room-discovery");t.setDaemon(true);return t;
    });
    private final AtomicBoolean scanning=new AtomicBoolean();
    private AtomicBoolean cancelScan=new AtomicBoolean();
    private LinearLayout hostPanel,joinPanel,results,manualPanel;
    private EditText hostName,roomName,hostPin,port,guestName,guestPin,ip,pinHash;
    private TextView status,scanStatus;
    private boolean closed;
    private int tab;
    private String lastPhase="";
    private final Runnable poll=new Runnable(){
        public void run(){
            if(closed)return;
            RoomSession.Status state=session.snapshot();
            String text=describe(state);
            if(!text.equals(status.getText().toString()))status.setText(text);
            String phase=state.phase.name();
            if(!phase.equals(lastPhase)){lastPhase=phase;RoomDiagnostics.event("ROOM_"+phase);}
            handler.postDelayed(this,400);
        }
    };
    public static void show(Activity activity,int action){
        if(action!=-102&&action!=-103)return;
        if(active!=null&&!active.closed&&active.activity==activity){
            active.select(action);return;
        }
        if(active!=null)active.dialog.dismiss();
        RoomDiagnostics.init(activity.getApplicationContext());
        RoomOverlay overlay=new RoomOverlay(activity);
        active=overlay;
        overlay.select(action);
        overlay.dialog.show();
        Window w=overlay.dialog.getWindow();
        if(w!=null){
            w.setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
            w.setLayout(-1,-1);
            w.addFlags(WindowManager.LayoutParams.FLAG_DIM_BEHIND);
            WindowManager.LayoutParams attrs=w.getAttributes();
            attrs.dimAmount=0.68f;w.setAttributes(attrs);
        }
        overlay.handler.post(overlay.poll);
        RoomDiagnostics.event("MENU_OPEN");
    }
    private RoomOverlay(Activity context){
        activity=context;session=RoomAppRooms.obtain();
        dialog=new Dialog(context);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        ScrollView scroll=new ScrollView(context);
        scroll.setFillViewport(true);
        LinearLayout root=new LinearLayout(context);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setPadding(dp(22),dp(14),dp(22),dp(22));
        root.setBackgroundColor(Color.rgb(14,15,17));
        scroll.addView(root);dialog.setContentView(scroll);
        label(root,"B L A C K    /    C O - O P",24,Color.rgb(176,55,47));
        label(root,"LOCAL NETWORK  —  SECURE ROOM",14,FG);
        label(root,"This is a real TLS lobby; human game actors and story sync are not ready.",12,MUTED);
        button(root,"HOST ROOM",()->select(-102));
        button(root,"FIND LAN ROOM",()->select(-103));
        hostPanel=group(root);
        label(hostPanel,"CREATE ROOM",18,FG);
        hostName=field(hostPanel,"PLAYER NAME","Host",32,false);hostName.setText("Host");
        roomName=field(hostPanel,"ROOM NAME","BLACK LAN",32,false);roomName.setText("BLACK LAN");
        hostPin=field(hostPanel,"PRIVATE 8-DIGIT PIN","",8,true);
        port=field(hostPanel,"PORT","24872",5,false);
        port.setInputType(InputType.TYPE_CLASS_NUMBER);port.setText("24872");
        button(hostPanel,"START HOST",this::host);
        joinPanel=group(root);
        label(joinPanel,"AVAILABLE WI-FI ROOMS",18,FG);
        guestName=field(joinPanel,"YOUR NAME","Player 2",32,false);guestName.setText("Player 2");
        guestPin=field(joinPanel,"ROOM'S 8-DIGIT PIN","",8,true);
        button(joinPanel,"REFRESH LAN",this::scan);
        scanStatus=label(joinPanel,"Looking for nearby rooms...",12,MUTED);
        results=group(joinPanel);
        button(joinPanel,"MANUAL JOIN (ADVANCED)",()->manualPanel.setVisibility(
            manualPanel.getVisibility()==View.GONE?View.VISIBLE:View.GONE));
        manualPanel=group(joinPanel);manualPanel.setVisibility(View.GONE);
        ip=field(manualPanel,"HOST IPv4:PORT","192.168.1.10:24872",21,false);
        pinHash=field(manualPanel,"VERIFIED HOST TLS SHA-256","",64,false);
        button(manualPanel,"JOIN VERIFIED ADDRESS",()->join(value(ip),value(pinHash)));
        label(root,"REAL CONNECTION STATUS",15,FG);
        status=label(root,"OFFLINE",14,FG);
        button(root,"LEAVE ROOM & BACK",()->{
            cancelScan.set(true);session.stop();RoomDiagnostics.event("ROOM_LEFT");dialog.dismiss();
        });
        button(root,"BACK TO GAME  ·  KEEP ROOM",()->dialog.dismiss());
        button(root,"COPY SAFE CONNECTION LOG",()->{
            ClipboardManager clip=(ClipboardManager)activity.getSystemService(Context.CLIPBOARD_SERVICE);
            if(clip!=null){clip.setPrimaryClip(ClipData.newPlainText("BLACK Co-op",RoomDiagnostics.read()));
                status.setText("Log copied; collect a copy from EACH device.");}
        });
        dialog.setOnDismissListener(ignored->{
            closed=true;cancelScan.set(true);worker.shutdownNow();handler.removeCallbacks(poll);
            if(active==this)active=null;
            RoomDiagnostics.event("MENU_BACK_TO_GAME");
        });
    }
    private int dp(int n){return (int)(n*activity.getResources().getDisplayMetrics().density+0.5f);}
    private GradientDrawable style(int color){
        GradientDrawable d=new GradientDrawable();
        d.setColor(color);d.setCornerRadius(dp(4));
        d.setStroke(dp(1),Color.rgb(83,65,63));return d;
    }
    private LinearLayout group(LinearLayout parent){
        LinearLayout view=new LinearLayout(activity);view.setOrientation(LinearLayout.VERTICAL);
        view.setPadding(dp(10),dp(8),dp(10),dp(8));
        view.setBackground(style(Color.rgb(29,29,31)));
        LinearLayout.LayoutParams lp=new LinearLayout.LayoutParams(-1,-2);
        lp.topMargin=dp(8);parent.addView(view,lp);return view;
    }
    private TextView label(LinearLayout parent,String text,int size,int color){
        TextView view=new TextView(activity);
        view.setText(text);view.setTextSize(size);view.setTextColor(color);
        view.setPadding(dp(4),dp(6),dp(4),dp(6));
        parent.addView(view);return view;
    }
    private void button(LinearLayout parent,String text,Runnable click){
        Button button=new Button(activity);
        button.setText(text);button.setAllCaps(false);button.setTextSize(13);
        button.setTextColor(FG);button.setBackground(style(Color.rgb(74,30,29)));
        LinearLayout.LayoutParams lp=new LinearLayout.LayoutParams(-1,dp(45));
        lp.topMargin=dp(5);parent.addView(button,lp);
        button.setOnClickListener(v->click.run());
    }
    private EditText field(LinearLayout parent,String title,String hint,int size,boolean secret){
        label(parent,title,11,MUTED);
        EditText field=new EditText(activity);
        field.setSingleLine(true);field.setHint(hint);field.setTextSize(15);
        field.setTextColor(FG);field.setHintTextColor(MUTED);
        field.setFilters(new InputFilter[]{new InputFilter.LengthFilter(size)});
        field.setInputType(secret?InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_VARIATION_PASSWORD:
            InputType.TYPE_CLASS_TEXT);
        field.setBackground(style(Color.rgb(13,14,16)));
        field.setPadding(dp(9),dp(6),dp(9),dp(6));
        parent.addView(field,new LinearLayout.LayoutParams(-1,dp(42)));return field;
    }
    private static String value(EditText field){return field.getText().toString().trim();}
    private void select(int action){
        tab=action;hostPanel.setVisibility(action==-102?View.VISIBLE:View.GONE);
        joinPanel.setVisibility(action==-103?View.VISIBLE:View.GONE);
        if(action==-103)scan();else cancelScan.set(true);
    }
    private void host(){
        String name=value(hostName),room=value(roomName),pin=value(hostPin);
        try{
            int portNumber=Integer.parseInt(value(port));
            RoomWire.name(name);RoomWire.name(room);
            if(portNumber<1024||portNumber>65535||!pin.matches("[0-9]{8}"))
                throw new IllegalArgumentException();
            session.startHost(()->{
                RoomIdentity identity=RoomIdentity.load();
                return new RoomSession.HostIdentity(identity.serverContext(),identity.fingerprint());
            },InetAddress.getByAddress(new byte[]{0,0,0,0}),portNumber,name,room,pin);
            RoomDiagnostics.event("HOST_REQUEST");
            status.setText("Starting encrypted LAN room...");
        }catch(Exception error){
            RoomDiagnostics.event("HOST_INPUT_ERROR");
            status.setText("Enter names, 8-digit PIN, and port 1024–65535.");
        }
    }
    private static String hex(byte[] data){
        StringBuilder out=new StringBuilder(64);
        for(byte b:data)out.append(String.format(Locale.ROOT,"%02X",b&255));
        return out.toString();
    }
    private void join(String address,String fingerprint){
        try{
            RoomWire.name(value(guestName));RoomDirect.parse(address);
            RoomDirect.fingerprint(fingerprint);
            if(!value(guestPin).matches("[0-9]{8}"))throw new IllegalArgumentException();
            session.join(address,fingerprint,value(guestName),value(guestPin));
            status.setText("Verifying host certificate and joining...");
            RoomDiagnostics.event("JOIN_REQUEST");
        }catch(Exception error){
            status.setText("Enter a valid room PIN, name, address and verified fingerprint.");
            RoomDiagnostics.event("JOIN_INPUT_ERROR");
        }
    }
    private void joinFound(RoomDiscovery.Found found){
        if(!found.compatible){status.setText("Incompatible BLACK game profile.");return;}
        if(found.occupied==4){status.setText("Room full.");return;}
        if(!value(guestPin).matches("[0-9]{8}")){
            status.setText("Enter the 8-digit room PIN then tap its name.");return;
        }
        String address=found.address.getHostAddress()+":"+found.port;
        String fingerprint=hex(found.fingerprint);
        String trusted=activity.getPreferences(Context.MODE_PRIVATE).getString("trusted_"+address,"");
        if(trusted.equals(fingerprint)){join(address,fingerprint);return;}
        // Discovery is UNAUTHENTICATED. First connection must confirm certificate
        // against the host phone's displayed fingerprint before sending the PIN.
        new AlertDialog.Builder(activity).setTitle("VERIFY HOST IDENTITY")
            .setMessage("Compare this full SHA-256 fingerprint with the HOST phone:\n\n"+
                fingerprint+"\n\nWi-Fi discovery alone is not authentication.")
            .setNegativeButton("CANCEL",null)
            .setPositiveButton("I VERIFIED THE MATCH",(dialog,which)->{
                activity.getPreferences(Context.MODE_PRIVATE).edit()
                    .putString("trusted_"+address,fingerprint).apply();
                RoomDiagnostics.event("HOST_VERIFIED");join(address,fingerprint);
            }).show();
    }
    private void scan(){
        if(closed||tab!=-103||!scanning.compareAndSet(false,true))return;
        cancelScan.set(true);
        AtomicBoolean cancelled=new AtomicBoolean();
        cancelScan=cancelled;results.removeAllViews();
        scanStatus.setText("Scanning current Wi-Fi...");
        RoomDiagnostics.event("SCAN_STARTED");
        worker.execute(()->{
            List<RoomDiscovery.Found> found=null;
            boolean failed=false;
            try{found=RoomDiscovery.scan(RoomDiscovery.broadcastTargets(),
                RoomDiscovery.PORT,1900,cancelled);}
            catch(Exception error){failed=true;RoomDiagnostics.event("SCAN_FAILED");}
            List<RoomDiscovery.Found> rooms=found;
            final boolean scanFailed=failed;
            handler.post(()->{
                scanning.set(false);
                if(closed||cancelled.get()||tab!=-103)return;
                int count=rooms==null?0:rooms.size();
                RoomDiagnostics.event("SCAN_RESULTS_"+Math.min(count,16));
                if(scanFailed){
                    scanStatus.setText("LAN search error. Check Wi-Fi connectivity and system restrictions.");
                }else if(count==0){
                    scanStatus.setText("No rooms found. Check same Wi-Fi / AP isolation, or use MANUAL JOIN.");
                }else{
                    scanStatus.setText(count+" room(s). Tap a room after entering its PIN.");
                    for(RoomDiscovery.Found room:rooms){
                        button(results,room.room+" · "+room.occupied+"/4"+
                            (room.compatible?" · JOIN":" · INCOMPATIBLE"),()->joinFound(room));
                    }
                }
            });
        });
    }
    private static String addresses(){
        StringBuilder result=new StringBuilder();
        try{
            Enumeration<NetworkInterface> interfaces=NetworkInterface.getNetworkInterfaces();
            while(interfaces!=null&&interfaces.hasMoreElements()){
                NetworkInterface nic=interfaces.nextElement();
                if(!nic.isUp()||nic.isLoopback())continue;
                Enumeration<InetAddress> ip=nic.getInetAddresses();
                while(ip.hasMoreElements()){
                    InetAddress candidate=ip.nextElement();
                    if(candidate instanceof Inet4Address&&!candidate.isLoopbackAddress()&&
                       !candidate.isAnyLocalAddress()&&!candidate.isLinkLocalAddress()){
                        if(result.length()>0)result.append(" / ");
                        result.append(candidate.getHostAddress());
                    }
                }
            }
        }catch(Exception ignored){}
        return result.toString();
    }
    private String describe(RoomSession.Status state){
        StringBuilder out=new StringBuilder("STATE: ").append(state.phase);
        if(!state.description.isEmpty())out.append("\n").append(state.description);
        if(state.phase==RoomSession.Phase.HOSTING)
            out.append("\nLOCAL IPv4: ").append(addresses()).append(":").append(state.port)
                .append("\nHOST SHA-256 (verify with guest):\n").append(state.fingerprint);
        if(state.roster!=null){
            out.append("\nAUTHENTICATED PLAYERS: ").append(state.roster.players.size()).append("/4");
            for(RoomWire.Player player:state.roster.players)
                out.append("\n").append(player.id).append(" · ").append(player.name);
            out.append("\nSTORY START: unavailable until real remote actors and story sync exist.");
        }
        return out.toString();
    }
}
