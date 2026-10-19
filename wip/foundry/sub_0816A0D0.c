#define SceneHud_DrawProjectedValue sub_0816A0D0
#define gGameState gUnk_02000710
#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
s32 Div(s32, s32);
void sub_0816A0D0(struct SceneHudState *p)
{
    struct SceneInputPackedPosition position; /* Only screen X/Y are consumed; the ROM also copies the unused Z/padding word. */
    s32 value;
    s32 digits[4];
    s32 *digit;
    struct SceneInputWidget *widget;
    s32 offset, i;
    position.x = 144;
    position.y = 40;
    value = *(s32 *)((u8 *)gUnk_02000710 + 0x50C) + p->valueRate * p->selectedQuantity;
    digits[0] = Div(value, 1000);
    value -= digits[0] * 1000;
    digits[1] = Div(value, 100);
    value -= digits[1] * 100;
    digits[2] = Div(value, 10);
    value -= digits[2] * 10;
    digits[3] = value;
    digit = digits;
    widget = &p->motionWidgets[2];
    offset = 0x1740;
    i = 3;
    do {
        *(u32 *)((u32)p + 0x1428 + offset) &= ~1;
        *(struct SceneInputPackedPosition *)((u32)p + 0x1440 + offset) = position;
        Anim_LoadKeyframe(widget, p->widgetResource, (u16)(*digit++ + 20), 1);
        position.x += 8;
        widget++;
        offset += 0x60;
    } while (--i >= 0);
}
