#include "global.h"
#include "scene_input.h"
#include "game_state.h"

s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);
s32 Div(s32, s32);

/* ControlPanel object: offsets established within this file; no shared header yet. */
struct S {
    u8 f1[0x378];
    u8 resource[0x3F8 - 0x378];
    struct SceneInputWidget digits[4];
};

void ControlPanel_ShowRewardBalanceDigits(struct S *p)
{
    s32 value = gGameState->menuRewardBalance;
    s32 digits[4];
    s32 *digit;
    s32 remaining;
    struct SceneInputWidget *widget;

    digits[0] = Div(value, 1000);
    value -= digits[0] * 1000;
    digits[1] = Div(value, 100);
    value -= digits[1] * 100;
    digits[2] = Div(value, 10);
    value -= digits[2] * 10;
    digits[3] = value;

    digit = digits;
    widget = p->digits;
    remaining = 3;
    do {
        Anim_LoadKeyframe(widget, p->resource, (u16)(*digit++ + 0x1E), 1);
        widget++;
        remaining--;
    } while (remaining >= 0);
}
