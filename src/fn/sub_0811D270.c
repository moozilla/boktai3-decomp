#include "global.h"
#ifdef NONMATCHING
// fuzzy: 81.8%
// foundry draft (worker best 81.8%); what differs is mostly register allocation and instruction order
#define SceneHud_DrawStyledDecimal sub_0811D270
#define SceneHud_GetLayerMapCell   Video_GetWrappedBackgroundMapEntry
s32 Div(s32, s32);
u16 *Video_GetWrappedBackgroundMapEntry(s32, u8 *, u32);
void sub_0811D270(register s32 value, s32 divisor, u8 *x, u32 y, s32 style)
{
    u16 *cell;
    s32 base = 0x21;
    if (!style)
        base = 0x11;
    cell = Video_GetWrappedBackgroundMapEntry(3, x, y);
    if (divisor > 0) {
        do {
            s32 digit = Div(value, divisor);
            *cell++ = digit + base;
            value -= digit * divisor;
            divisor = Div(divisor, 10);
        } while (divisor > 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811D270);
#endif
