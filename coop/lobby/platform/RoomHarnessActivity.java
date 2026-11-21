// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

/** Test-only game-activity surrogate. No copyrighted assets or SDL native code. */
public final class RoomHarnessActivity extends Activity {
    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        TextView root=new TextView(this);
        root.setText("BLACK room Back test harness");
        setContentView(root);
    }
}
