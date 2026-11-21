// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

/**
 * Fail-closed discovery policy. LAN scans are only useful while the owner
 * is searching for a room. Never keep broadcasting when authenticated,
 * joining, or hosting (CL two-phone log showed 70 idle discovery requests
 * even after successful JOINED).
 *
 * Pure Java, tested on JVM; no Android, game memory, or socket ownership.
 */
final class RoomScanGate {
    private RoomScanGate() {}

    static boolean canSearch(RoomSession.Phase phase) {
        return phase==RoomSession.Phase.IDLE || phase==RoomSession.Phase.ERROR;
    }

    static boolean shouldAutoScan(int selectedTab, RoomSession.Phase phase,
                                  boolean scanInProgress, long elapsedMs,
                                  long earliestNextMs) {
        return selectedTab==-103 && canSearch(phase) && !scanInProgress &&
               elapsedMs>=earliestNextMs;
    }

    static boolean canStartRoom(RoomSession.Phase phase) {
        // Explicit LEAVE is required before rehosting or switching roles.
        // Avoid accidental duplicate binds and double-join while connected.
        return canSearch(phase);
    }
}
