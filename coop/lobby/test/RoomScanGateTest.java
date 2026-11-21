// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

/** CL two-device scan storm regression, no real phone required. */
public final class RoomScanGateTest {
    private static void check(boolean ok) {
        if(!ok)throw new AssertionError("Discovery phase fence");
    }
    public static void main(String[] args) {
        int roomTab=-103;
        check(RoomScanGate.shouldAutoScan(roomTab,RoomSession.Phase.IDLE,false,5500,5500));
        check(RoomScanGate.shouldAutoScan(roomTab,RoomSession.Phase.ERROR,false,5600,5500));
        check(!RoomScanGate.shouldAutoScan(roomTab,RoomSession.Phase.IDLE,true,7000,5500));
        check(!RoomScanGate.shouldAutoScan(roomTab,RoomSession.Phase.IDLE,false,5499,5500));
        check(!RoomScanGate.shouldAutoScan(-102,RoomSession.Phase.IDLE,false,6000,5500));
        for(RoomSession.Phase active : new RoomSession.Phase[]{
            RoomSession.Phase.CONNECTING,RoomSession.Phase.JOINED,RoomSession.Phase.HOSTING
        }) {
            check(!RoomScanGate.canSearch(active));
            check(!RoomScanGate.canStartRoom(active));
            check(!RoomScanGate.shouldAutoScan(roomTab,active,false,999_999,0));
        }
        check(RoomScanGate.canStartRoom(RoomSession.Phase.IDLE));
        check(RoomScanGate.canStartRoom(RoomSession.Phase.ERROR));
        System.out.println("ROOM_SCAN_GATE_PASS: no discovery while joining, joined or hosting; can re-search after leave/error");
    }
}
