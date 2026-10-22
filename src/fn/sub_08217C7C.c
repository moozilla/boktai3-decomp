#include "global.h"

#ifdef NONMATCHING
// fuzzy: 86.9%
// Remaining difference: agbcc hoists the 0x03004FC0 address out of the first loop (ROM reloads it each
// outer iteration and keeps the buffer in ip), which shifts the registers of the loop.
extern u16 *Palette_GetBackgroundBuffer(void);
extern u16 *gUnk_030051E0;
extern u16 gUnk_03004FC0[][16];
extern s16 gUnk_03004DB0, gUnk_030051D0;
void CpuSet(const void *, void *, u32);

static inline u32 Gray(s32 color)
{
    s32 r = color & 0x1F;
    s32 g = (color & 0x3E0) >> 5;
    s32 b = (color & 0x7C00) >> 10;
    s32 v = r + g + b;
    v = (v >> 2) + (v >> 4) + (v >> 6);
    return v | (v << 5) | (v << 10);
}

void sub_08217C7C(void)
{
    u16(*buffer)[16] = (void *)Palette_GetBackgroundBuffer();
    s32 i, j;

    gUnk_030051E0 = (u16 *)buffer;
    for (i = 0; i <= 12; i++) {
        for (j = 0; j < 16; j++) {
            (*(buffer + i))[j] = Gray((*(gUnk_03004FC0 + i))[j]);
        }
    }
    for (i = 13; i < 16; i++) {
        CpuSet(gUnk_03004FC0[i], buffer[i], 0x04000008);
    }
    if (gUnk_03004DB0) {
        gUnk_030051D0 = Gray(gUnk_030051D0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08217C7C);
#endif
