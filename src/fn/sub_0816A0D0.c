#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
s32 Div(s32, s32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void sub_0816A0D0(struct SceneHudState *p)
{
    struct ContactPosition position;
    s32 digits[4];
    s32 value;
    s32 *digit;
    s32 i;
    position.x = 144;
    position.y = 40;
    value = gGameState->menuRewardBalance + p->valueRate * p->selectedQuantity;
    digits[0] = Div(value, 1000);
    value -= digits[0] * 1000;
    digits[1] = Div(value, 100);
    value -= digits[1] * 100;
    digits[2] = Div(value, 10);
    value -= digits[2] * 10;
    digits[3] = value;
    digit = digits;
    /* categoryWidgets[62 + i] is motionWidgets[2 + i]; the ROM addresses it both ways */
    for (i = 0; i < 4; i++) {
        p->categoryWidgets[i + 62].flags &= ~1;
        p->categoryWidgets[i + 62].position = position;
        Anim_LoadKeyframe(&p->motionWidgets[i + 2], p->widgetResource, (u16)(*digit++ + 20), 1);
        position.x += 8;
    }
}
