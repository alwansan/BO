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
    return 0;
}
