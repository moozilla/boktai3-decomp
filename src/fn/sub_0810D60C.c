
#ifdef NONMATCHING
// fuzzy: 81.8%
// foundry draft (worker best 81.8%); what differs is mostly register allocation and instruction order
#define PanelText_DrawDecimal sub_0810D60C
#define PanelText_GetMapCell  Bg_GetWrappedTilemapEntry
#include "global.h"
s32 Div(s32, s32);
u8 *Bg_GetWrappedTilemapEntry(u32, u32, u32);
void sub_0810D60C(s32 value, s32 divisor, u32 x, u32 y, s32 style)
{
    s32 base = 0x21;
    u16 *cell;
    if (style == 0)
        base = 0x11;
    cell = (u16 *)Bg_GetWrappedTilemapEntry(3, x, y);
    while (divisor > 0) {
        s32 digit = Div(value, divisor);
        *cell = digit + base;
        cell++;
        value = value - digit * divisor;
        divisor = Div(divisor, 10);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810D60C);
#endif
