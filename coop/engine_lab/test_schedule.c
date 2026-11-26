/* Harness accounting only. This does not execute a game function. */
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "schedule.h"
int main(void) {
    unsigned char report[20];
    BoLabInput in;
    memset(report, 0xCC, sizeof(report));
    in = bo_lab_input(0, 1000, 0, 0);
    assert(in.confirm && !in.move && !in.fire);
    in = bo_lab_input(1, 1000, 0, 0);
    assert(!in.confirm && !in.start && !in.move && !in.fire);
    in = bo_lab_input(0, 0, 1, 0);
    assert(!in.confirm && !in.start && !in.move && !in.fire);
    in = bo_lab_input(0, 0, 1, 1);
    assert(in.move == 1 && !in.fire);
    in = bo_lab_input(1, 0, 1, 1);
    assert(in.move == 0 && !in.fire);
    in = bo_lab_input(1, 0, 1, 3);
    assert(in.move == -1 && !in.fire);
    in = bo_lab_input(0, 0, 1, 5);
    assert(in.fire && !in.move);
    in = bo_lab_input(1, 0, 1, 7);
    assert(in.fire && !in.move);
    in = bo_lab_input(0, 68000, 0, 0);
    assert(in.start && !in.confirm);
    assert(bo_lab_ui_category("blacktitlemenu2") == 1);
    assert(bo_lab_ui_category("blacktitlemenu") == 2);
    assert(bo_lab_ui_category("Difficulty") == 4);
    assert(bo_lab_ui_category("ProfileOptions") == 5);
    assert(bo_lab_ui_category("UnknownPrivatePage") == 7);
    bo_lab_encode(bo_lab_input(1, 0, 1, 3), report);
    assert(report[1] == 20 && report[12] == 0 && report[13] == 0x80);
    assert(report[11] == 0 && report[4] == 0 && report[2] == 0);
    int armed = 0;
    assert(bo_lab_restart_menu("ConfirmWarning", 0, &armed) == BO_RESTART_NEUTRAL);
    assert(bo_lab_restart_menu("PauseMenu", 2, &armed) == BO_RESTART_DOWN);
    assert(bo_lab_restart_menu("PauseMenu", 4, &armed) == BO_RESTART_UP);
    assert(bo_lab_restart_menu("PauseMenu", 3, &armed) == BO_RESTART_CONFIRM);
    assert(bo_lab_restart_menu("ConfirmWarning", 1, &armed) == BO_RESTART_UP);
    assert(bo_lab_restart_menu("ConfirmWarning", 0, &armed) == BO_RESTART_CONFIRM);
    assert(bo_lab_restart_menu("Other", 0, &armed) == BO_RESTART_NEUTRAL && !armed);
    assert(!bo_lab_restart_open_input(0, 0).start);
    for (unsigned attempt = 0; attempt < 4; ++attempt) {
        const unsigned first = 1 + attempt * 6;
        assert(bo_lab_restart_open_input(0, first).start);
        assert(bo_lab_restart_open_input(0, first + 2).start);
        assert(!bo_lab_restart_open_input(0, first + 3).start);
        assert(!bo_lab_restart_open_input(0, first + 5).start);
        assert(!bo_lab_restart_open_input(1, first).start);
    }
    assert(!bo_lab_restart_open_input(0, BO_RESTART_OPEN_FRAMES + 1).start);
    bo_lab_encode(bo_lab_restart_open_input(0, 2), report);
    assert(report[2] == 0x10 && report[4] == 0);
    return 0;
}
