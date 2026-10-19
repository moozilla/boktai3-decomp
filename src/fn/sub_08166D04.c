
#ifdef NONMATCHING
// fuzzy: 96.2%
// foundry draft (worker best 96.2%); what differs is mostly register allocation and instruction order
#define MenuDisplay_LoadDecimalDigits sub_08166D04
#include "global.h"
struct MenuDigit { u8 animation[0x60]; };
struct MenuDisplay {
    u8 unknown0[0x64];
    u8 animationData[0x13BC];
    struct MenuDigit digits[1];
};
s32 Div(s32, s32);
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
void sub_08166D04(struct MenuDisplay *p, s32 value, s32 divisor, s32 frameBase, s32 first)
{
    struct MenuDigit *digit;
    if (value > 999)
        value = 999;
    for (; divisor > 0; first++) {
        s32 quotient = Div(value, divisor);
        digit = p->digits;
        Anim_SelectWidgetGlyph(&digit[first], p->animationData, (u16)(frameBase + quotient));
        value -= quotient * divisor;
        divisor = Div(divisor, 10);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08166D04);
#endif
