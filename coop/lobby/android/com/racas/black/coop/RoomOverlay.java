// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import android.app.Activity;
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
    private EditText hostName,roomName,guestName,ip;
    private TextView status,scanStatus,notice;
    private boolean closed;
    private int tab;
    private String lastPhase="";
    private long nextScan;
    private final Runnable poll=new Runnable(){
        public void run(){
            if(closed)return;
            RoomSession.Status state=session.snapshot();
            String text=describe(state);
            if(!text.equals(status.getText().toString()))status.setText(text);
            String phase=state.phase.name();
            if(!phase.equals(lastPhase)){lastPhase=phase;RoomDiagnostics.event("ROOM_"+phase);}
            if(!RoomScanGate.canSearch(state.phase) && !cancelScan.get()){
                // Stop the previous untrusted discovery task after TLS connects.
                cancelScan.set(true);
                if(tab==-103)scanStatus.setText("تم الاتصال؛ لا حاجة للبحث عن غرف أخرى.");
                RoomDiagnostics.event("SCAN_PAUSED_ACTIVE_ROOM");
            }
            if(RoomScanGate.shouldAutoScan(tab,state.phase,scanning.get(),
                    android.os.SystemClock.elapsedRealtime(),nextScan))scan();
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
    static void startHostForTest(){
        if(active==null)throw new IllegalStateException("Room dialog closed");
        View button=active.dialog.getWindow().getDecorView().findViewWithTag("room_host_start");
        if(button==null||!button.performClick())throw new IllegalStateException("Host button missing");
    }
    private RoomOverlay(Activity context){
        activity=context;RoomAppRooms.attach(context);session=RoomAppRooms.obtain();
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
        label(root,"الشبكة المحلية / VPN",14,FG);
        label(root,"اتصال الأجهزة والغرف؛ مزامنة الشخصيات والقصة قيد التطوير.",12,MUTED);
        LinearLayout tabs=new LinearLayout(context);
        tabs.setOrientation(LinearLayout.HORIZONTAL);
        root.addView(tabs);
        button(tabs,"استضافة",()->select(-102));
        button(tabs,"الأجهزة والغرف",()->select(-103));
        // A real landscape game needs two columns, not a tall Android settings page.
        boolean wide=context.getResources().getConfiguration().screenWidthDp>=600;
        LinearLayout sections=new LinearLayout(context);
        sections.setOrientation(wide?LinearLayout.HORIZONTAL:LinearLayout.VERTICAL);
        root.addView(sections,new LinearLayout.LayoutParams(-1,-2));
        LinearLayout controls=new LinearLayout(context);
        controls.setOrientation(LinearLayout.VERTICAL);
        LinearLayout side=new LinearLayout(context);
        side.setOrientation(LinearLayout.VERTICAL);
        if(wide){
            sections.addView(controls,new LinearLayout.LayoutParams(0,-2,2f));
            sections.addView(side,new LinearLayout.LayoutParams(0,-2,1f));
        }else{
            sections.addView(controls,new LinearLayout.LayoutParams(-1,-2));
            sections.addView(side,new LinearLayout.LayoutParams(-1,-2));
        }
        hostPanel=group(controls);
        label(hostPanel,"إنشاء غرفة",18,FG);
        hostName=field(hostPanel,"اسمك","Host",32,false);hostName.setText(RoomAppRooms.displayName());
        roomName=field(hostPanel,"اسم الغرفة","BLACK LAN",32,false);roomName.setText("BLACK LAN");
        button(hostPanel,"بدء الاستضافة",this::host).setTag("room_host_start");
        joinPanel=group(controls);
        label(joinPanel,"الأجهزة المفتوحة والغرف المتاحة",18,FG);
        guestName=field(joinPanel,"اسمك","Player",32,false);guestName.setText(RoomAppRooms.displayName());
        button(joinPanel,"تحديث الآن",this::scan);
        scanStatus=label(joinPanel,"جارٍ اكتشاف الأجهزة تلقائياً…",12,MUTED);
        results=group(joinPanel);
        button(joinPanel,"اتصال يدوي عند الحاجة",()->manualPanel.setVisibility(
            manualPanel.getVisibility()==View.GONE?View.VISIBLE:View.GONE));
        manualPanel=group(joinPanel);manualPanel.setVisibility(View.GONE);
        ip=field(manualPanel,"عنوان جهاز المضيف IPv4","192.168.1.10",21,false);
        button(manualPanel,"انضمام",()->join(value(ip)));
        LinearLayout statusPanel=group(side);
        label(statusPanel,"حالة الاتصال",15,FG);
        status=label(statusPanel,"OFFLINE",14,FG);
        notice=label(statusPanel,"اختر الاستضافة أو جهازاً من القائمة.",12,MUTED);
        button(statusPanel,"مغادرة الغرفة والرجوع",()->{
            cancelScan.set(true);session.stop();RoomDiagnostics.event("ROOM_LEFT");dialog.dismiss();
        });
        button(statusPanel,"الرجوع للعبة مع إبقاء الاتصال",()->dialog.dismiss());
        button(statusPanel,"SAVE ROOM LOG TO DOWNLOADS",()->{
            RoomDiagnostics.event("EXPORT_REQUESTED");
            worker.execute(()->{
                boolean result=RoomDiagnostics.exportToDownloads(activity.getApplicationContext());
                handler.post(()->{
                    if(closed)return;
                    RoomDiagnostics.event(result?"EXPORT_OK":"EXPORT_FAILED");
                    if(result)notice.setText("Saved safe room events to Downloads/Games/cl/black log/logs. Share from BOTH phones.");
                    else notice.setText("Could not export room events; use COPY SAFE CONNECTION LOG.");
                });
            });
        });
        button(statusPanel,"COPY SAFE CONNECTION LOG",()->{
            ClipboardManager clip=(ClipboardManager)activity.getSystemService(Context.CLIPBOARD_SERVICE);
            if(clip!=null){clip.setPrimaryClip(ClipData.newPlainText("BLACK Co-op",RoomDiagnostics.read()));
                notice.setText("Log copied; collect a copy from EACH device.");}
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
    private Button button(LinearLayout parent,String text,Runnable click){
        Button button=new Button(activity);
        button.setText(text);button.setAllCaps(false);button.setTextSize(13);
        button.setTextColor(FG);button.setBackground(style(Color.rgb(74,30,29)));
        boolean horizontal=parent.getOrientation()==LinearLayout.HORIZONTAL;
        LinearLayout.LayoutParams lp=horizontal?
            new LinearLayout.LayoutParams(0,dp(45),1f):
            new LinearLayout.LayoutParams(-1,dp(45));
        lp.topMargin=dp(5);parent.addView(button,lp);
        button.setOnClickListener(v->click.run());return button;
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
        if(action==-103 && RoomScanGate.canSearch(session.snapshot().phase))scan();
        else{
            cancelScan.set(true);
            if(action==-103)scanStatus.setText("متصل بغرفة؛ غادر الغرفة قبل البحث من جديد.");
        }
    }
    private void host(){
        if(!RoomScanGate.canStartRoom(session.snapshot().phase)){
            notice.setText("هناك غرفة قيد الاتصال؛ غادرها أولاً قبل إنشاء غرفة جديدة.");
            RoomDiagnostics.event("HOST_BLOCKED_ALREADY_ACTIVE");return;
        }
        try{
            String name=RoomWire.name(value(hostName)),room=RoomWire.name(value(roomName));
            RoomAppRooms.setDisplayName(name);
            session.startHost(()->{RoomIdentity identity=RoomIdentity.load();return new RoomSession.HostIdentity(identity.serverContext(),identity.fingerprint());},InetAddress.getByAddress(new byte[]{0,0,0,0}),RoomDirect.DEFAULT_PORT,name,room);
            RoomDiagnostics.event("HOST_REQUEST");notice.setText("جارٍ فتح الغرفة…");
        }catch(Exception error){RoomDiagnostics.event("HOST_INPUT_ERROR");notice.setText("أدخل اسماً قصيراً للاعب والغرفة.");}
    }
    private void join(String address){
        if(!RoomScanGate.canStartRoom(session.snapshot().phase)){
            notice.setText("هناك اتصال نشط؛ غادر الغرفة أولاً قبل الانضمام لغيرها.");
            RoomDiagnostics.event("JOIN_BLOCKED_ALREADY_ACTIVE");return;
        }
        try{
            String name=RoomWire.name(value(guestName));RoomDirect.manual(address);RoomAppRooms.setDisplayName(name);
            session.joinAutomatic(address,name);cancelScan.set(true);
            RoomDiagnostics.event("JOIN_REQUEST");notice.setText("جارٍ الانضمام تلقائياً…");
        }catch(Exception error){notice.setText("أدخل اسمك وعنوان IP صحيحاً للمضيف.");RoomDiagnostics.event("JOIN_INPUT_ERROR");}
    }
    private void scan(){
        if(closed||tab!=-103||!RoomScanGate.canSearch(session.snapshot().phase)
           ||!scanning.compareAndSet(false,true))return;
        nextScan=android.os.SystemClock.elapsedRealtime()+5500;
        cancelScan.set(true);AtomicBoolean cancelled=new AtomicBoolean();cancelScan=cancelled;
        scanStatus.setText("جارٍ البحث عن الأجهزة والغرف…");RoomDiagnostics.event("SCAN_STARTED");
        worker.execute(()->{
            List<RoomPresence.Peer> found=null;boolean failed=false;
            try{found=RoomPresence.scan(RoomPresence.targets(),RoomPresence.PORT,4000,RoomAppRooms.presenceId(),cancelled);}catch(Exception error){failed=true;RoomDiagnostics.event("SCAN_FAILED");}
            List<RoomPresence.Peer> peers=found;final boolean scanFailed=failed;
            handler.post(()->{
                scanning.set(false);
                if(closed||cancelled.get()||tab!=-103||
                   !RoomScanGate.canSearch(session.snapshot().phase))return;
                results.removeAllViews();int count=peers==null?0:peers.size();RoomDiagnostics.event("SCAN_RESULTS_"+Math.min(count,32));
                if(scanFailed)scanStatus.setText("تعذر البحث الآن. يمكنك استخدام عنوان IP يدوياً.");
                else if(count==0)scanStatus.setText("لا أجهزة ظاهرة حالياً؛ البحث يتجدد تلقائياً. يمكنك استخدام عنوان IP.");
                else{
                    scanStatus.setText("أجهزة اللعبة المتاحة: "+count);
                    for(RoomPresence.Peer peer:peers){
                        String name=peer.info.name;
                        if(!peer.compatible){label(results,name+" — حدّث اللعبة على الجهازين",13,MUTED);continue;}
                        if(peer.info.role==1){
                            String target=peer.address.getHostAddress()+":"+peer.info.port;
                            Button choice=button(results,name+" · "+peer.info.room+" · "+peer.info.occupied+"/4 — انضمام",()->join(target));choice.setEnabled(peer.info.occupied<4);
                        }else label(results,name+(peer.info.role==2?" — متصل بغرفة":" — متاح، لم ينشئ غرفة"),14,FG);
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
        if(state.phase==RoomSession.Phase.HOSTING)out.append("\nعنوان المضيف للاتصال اليدوي: ").append(addresses());
        if(state.roster!=null){
            out.append("\nاللاعبون المتصلون: ").append(state.roster.players.size()).append("/4");
            for(RoomWire.Player player:state.roster.players)
                out.append("\n").append(player.id).append(" · ").append(player.name);
            out.append("\nSTORY START: unavailable until real remote actors and story sync exist.");
        }
        return out.toString();
    }
}
