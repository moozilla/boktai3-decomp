#include "global.h"

extern u16 *Palette_GetBackgroundBuffer(void);
extern u16 *gBgPaletteSource;
extern u16 gUnk_03004FC0[][16];
extern s16 gBackdropOverride, gBackdropColor;
void CpuSet(const void *, void *, u32);

static inline u32 Gray(s32 color)
{
    s32 r = color & 0x1F;
    s32 g = (color & 0x3E0) >> 5;
    s32 b = (color & 0x7C00) >> 10;
    s32 v = r;
    v += g; // accumulating keeps the loop body large enough that the masks stay inside
    v += b;
    v = (v >> 2) + (v >> 4) + (v >> 6);
    return v | (v << 5) | (v << 10);
}

void sub_08217C7C(void)
{
    u16(*buffer)[16] = (void *)Palette_GetBackgroundBuffer();
    s32 i, j;

    gBgPaletteSource = (u16 *)buffer;
    for (i = 0; i <= 12; i++) {
        u16 *src = gUnk_03004FC0[i];
        u16 *dst = buffer[i];
        for (j = 0; j < 16; j++) {
            *dst = Gray(*src);
            src++;
            dst++;
        }
    }
    for (i = 13; i < 16; i++) {
        CpuSet(gUnk_03004FC0[i], buffer[i], 0x04000008);
    }
    if (gBackdropOverride) {
        gBackdropColor = Gray(gBackdropColor);
    }
}
