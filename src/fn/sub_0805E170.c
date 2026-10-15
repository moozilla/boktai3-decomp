#include "scene_hud_display.h"
#define SceneHud_HideFourDigits sub_0805E170
void SceneHud_HideFourDigits(struct SceneHudDigitView *parent)
{
    struct SceneInputWidget *widget = &parent->label;
    u32 hidden = 1;
    s32 remaining = 4;
    do {
        widget->flags |= hidden;
        widget++;
    } while (--remaining >= 0);
}
