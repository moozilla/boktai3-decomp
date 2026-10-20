#include "global.h"

#ifdef NONMATCHING
// fuzzy: 86.6%
// Remaining differences: instruction selection, load placement, and literal-mask materialization.
// confidence=low: gauge source is the save-state counter at 0x618.
extern u8 *gUnk_02000710;
extern u16 gUnk_0300517C;
extern u16 gUnk_0300517A;
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Div(s32, s32);
void StatDisplay_DrawTwoDigitCounter(s32);
void Hud_DrawStoredEnergyGauge(u8 *p, s32 mode)
{
    s32 background, foreground, full, partial, n, amount;
    u16 *cell;
    s32 mask;
    if (mode == 1) {
        background = 0x1f;
        if (*(s16 *)(p + 0xa88) < *(s32 *)(gUnk_02000710 + 0x618)) {
            u16 *timer = (u16 *)(p + 0xa8a);
            ++*timer;
            foreground = 0x7fff;
            if ((*timer >> 2) & 1)
                foreground = 0x3e0;
        } else if (*(u16 *)(p + 0xa8c)) {
            foreground = 0x7fff;
            --*(u16 *)(p + 0xa8c);
        } else
            foreground = 0x3e0;
    } else {
        background = 0x39ce;
        if (*(u16 *)(p + 0xa8c)) {
            foreground = 0x7fff;
            --*(u16 *)(p + 0xa8c);
        } else
            foreground = 0x1f;
    }
    if (gUnk_0300517C != background)
        gUnk_0300517C = background;
    if ((&gUnk_0300517C)[-1] != foreground)
        (&gUnk_0300517C)[-1] = foreground;
    *(u16 *)(p + 0xa88) = *(s32 *)(gUnk_02000710 + 0x618);
    amount = *(s32 *)(gUnk_02000710 + 0x618);
    if (amount <= 0) {
        full = 0;
        partial = 0;
    } else if (*(s32 *)(gUnk_02000710 + 0x61c) > 1) {
        s32 scale = 79;
        partial = Div((amount - 1) * scale, *(s32 *)(gUnk_02000710 + 0x61c) - 1) + 1;
        full = partial >> 3;
        partial -= full << 3;
    } else {
        full = 10;
        partial = 0;
    }
    cell = Hud_GetTilemapCell(0, 29, 1);
    *cell-- = 0xd08c;
    *cell-- = 0xd08b;
    *cell-- = 0xd08a;
    for (n = 0; n < full; n++)
        *cell-- = 0xd089;
    if (partial) {
        mask = 0xffffd000;
        *cell-- = (partial + 0x81) | mask;
        n++;
    }
    for (; n < 10; n++)
        *cell-- = 0xd04b;
    *cell = 0xd44c;
    StatDisplay_DrawTwoDigitCounter(-1);
}

#else
INCLUDE_ASM("asm/nonmatching", Hud_DrawStoredEnergyGauge);
#endif
