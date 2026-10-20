#include "global.h"
#include "scene_hud_display.h"

s32 Div(s32, s32);

/* SceneHudState viewed as one widget array from 0x1420 (indices 75 and 76 are valueWidgets). */
struct SceneHudWidgetTable {
    u8 unknown0[0x1420];
    struct SceneInputWidget widgets[0x80];
};
#define WIDGET(p, n) (((struct SceneHudWidgetTable *)(p))->widgets[n])

void PauseMenu_RefreshPriceDigits(struct SceneHudState *p)
{
    s32 digits[4];
    s32 value, remaining, i;
    s32 *digit, *quantityDigit;
    void *data;

    value = p->valueRate;
    if (value == 0) {
        data = p->unknownC4;
        {
            s32 zero = 0;
            s32 *clear = &digits[3];
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)digits);
        }
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
        data = p->unknownC4;
    }
    digit = digits;
    for (i = 0; i <= 3; i++) {
        WIDGET(p, i + 113).flags &= ~1;
        Anim_LoadKeyframe(&WIDGET(p, i + 113), data, (u16)(*digit++ + 10), 1);
    }
    *(u32 *)((u8 *)p + 0x4008) &= ~1;
    value = p->valueRate * p->selectedQuantity;
    if (value == 0) {
        {
            s32 zero = 0;
            s32 *clear = &digits[3];
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)digits);
        }
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
    }
    quantityDigit = digits;
    for (i = 0; i <= 3; i++) {
        WIDGET(p, i + 118).flags &= ~1;
        Anim_LoadKeyframe(&WIDGET(p, i + 118), data, (u16)(*quantityDigit++ + 10), 1);
    }
}
