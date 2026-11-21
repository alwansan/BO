package com.racas.black.coop;
public final class RoomLobbyActivity extends android.app.Activity {
 @Override public void onCreate(android.os.Bundle state){super.onCreate(state);RoomOverlay.show(this,-102);}
 @Override public void onDestroy(){RoomAppRooms.shutdown();super.onDestroy();}
}
