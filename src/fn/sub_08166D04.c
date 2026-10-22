#include "global.h"

struct MenuDigit {
    u8 animation[0x60];
};
struct MenuDisplay {
    u8 unknown0[0x64];
    u8 animationData[0x13BC];
    struct MenuDigit digits[1];
};
s32 Div(s32, s32);
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
void MenuDisplay_LoadDecimalDigits(struct MenuDisplay *p, s32 value, s32 divisor, s32 frameBase,
                                   s32 first)
{
    s32 i;
    if (value > 999)
        value = 999;
    for (i = 0; divisor > 0; i++) {
        s32 quotient = Div(value, divisor);
        Anim_SelectWidgetGlyph(&p->digits[first + i], p->animationData,
                               (u16)(frameBase + quotient));
        value -= quotient * divisor;
        divisor = Div(divisor, 10);
    }
}
