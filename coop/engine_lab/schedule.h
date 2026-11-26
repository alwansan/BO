/* MIT. Test driver for the existing 20-byte original-Xbox USB report. */
#ifndef BO_ENGINE_LAB_SCHEDULE_H
#define BO_ENGINE_LAB_SCHEDULE_H
#include <stdint.h>
#include <string.h>
typedef struct BoLabInput { int confirm, start, move, fire, menu_dir; } BoLabInput;
enum BoLabRestartAction {
    BO_RESTART_NORMAL, BO_RESTART_NEUTRAL, BO_RESTART_OPEN,
    BO_RESTART_UP, BO_RESTART_DOWN, BO_RESTART_CONFIRM
};
static int bo_lab_prefix(const char *text, const char *prefix)
{
    for (; *prefix; ++prefix, ++text) {
        unsigned char a = (unsigned char)*text, b = (unsigned char)*prefix;
        if (a >= 'A' && a <= 'Z') a = (unsigned char)(a + 32);
        if (b >= 'A' && b <= 'Z') b = (unsigned char)(b + 32);
        if (a != b) return 0;
    }
    return 1;
}
/* Fixed coarse categories only. No page strings are emitted. */
static unsigned bo_lab_ui_category(const char *page)
{
    if (!page || !*page) return 0;
    if (bo_lab_prefix(page, "blacktitlemenu2")) return 1;
    if (bo_lab_prefix(page, "blacktitlemenu")) return 2;
    if (bo_lab_prefix(page, "OptionsMenuXbox")) return 3;
    if (bo_lab_prefix(page, "Difficulty")) return 4;
    if (bo_lab_prefix(page, "Profile") || bo_lab_prefix(page, "Name") || bo_lab_prefix(page, "Keyboard")) return 5;
    if (bo_lab_prefix(page, "Pause")) return 6;
    return 7;
}
static BoLabInput bo_lab_input(unsigned port, uint64_t elapsed_ms, int mission, unsigned phase)
{
    BoLabInput in = {0};
    if (port > 1) return in;
    if (!mission) {
        /* Fresh original profile/menu: confirm selected default rows. Bounded,
         * unverified navigation, never a direct mission-state write. */
        if (port == 0) {
            in.confirm = elapsed_ms >= 1000 && elapsed_ms % 2000 >= 1000 && elapsed_ms % 2000 < 1250;
            in.start = elapsed_ms % 12000 >= 8000 && elapsed_ms % 12000 < 8250;
        }
        return in;
    }
    if (phase == 1 && port == 0) in.move = 1;
    if (phase == 3 && port == 1) in.move = -1;
    if (phase == 5 && port == 0) in.fire = 1;
    if (phase == 7 && port == 1) in.fire = 1;
    return in;
}
/* Row indices from the pinned PauseMenu and ConfirmWarning definitions.
 * Never confirm an unknown or unexpected page. Confirmation must follow
 * actually seeing Restart Mission focused in the pause menu. */
static int bo_lab_restart_menu(const char *page, int focus, int *armed)
{
    if (page && !strcmp(page, "PauseMenu") && focus >= 0 && focus < 7) {
        *armed = focus == 3;
        if (*armed) return BO_RESTART_CONFIRM;
        return focus < 3 ? BO_RESTART_DOWN : BO_RESTART_UP;
    }
    if (*armed && page && !strcmp(page, "ConfirmWarning")) {
        if (focus == 0) return BO_RESTART_CONFIRM;
        if (focus == 1) return BO_RESTART_UP;
    }
    *armed = 0;
    return BO_RESTART_NEUTRAL;
}
static BoLabInput bo_lab_restart_input(unsigned port, uint64_t now_ms, int action)
{
    BoLabInput in = {0};
    if (port != 0 || now_ms % 1000u >= 250u) return in;
    in.start = action == BO_RESTART_OPEN;
    in.confirm = action == BO_RESTART_CONFIRM;
    in.menu_dir = action == BO_RESTART_UP ? -1 : action == BO_RESTART_DOWN ? 1 : 0;
    return in;
}
static void bo_lab_encode(BoLabInput in, unsigned char out[20])
{
    memset(out, 0, 20);
    out[1] = 20;
    out[2] = (in.start ? 0x10 : 0) | (in.menu_dir < 0 ? 0x01 : in.menu_dir > 0 ? 0x02 : 0);
    out[4] = in.confirm ? 255 : 0;
    out[11] = in.fire ? 255 : 0;
    if (in.move) { out[12] = in.move > 0 ? 0xFF : 0; out[13] = in.move > 0 ? 0x7F : 0x80; }
}
#endif
