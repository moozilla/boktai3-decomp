#include "global.h"
#include "engine/video/render.h"

extern u16 *Palette_GetBackgroundBuffer(void);
void CpuSet(const void *, void *, u32);

static inline u32 Gray(s32 color)
{
    s32 r = color & 0x1F;
    s32 g = (color & 0x3E0) >> 5;
    s32 b = (color & 0x7C00) >> 10;
    s32 gray = r;
    gray += g;
    gray += b;
    gray = (gray >> 2) + (gray >> 4) + (gray >> 6);
    return gray | (gray << 5) | (gray << 10);
}

void sub_08217C7C(void)
{
    u16(*buffer)[16] = (void *)Palette_GetBackgroundBuffer();
    s32 i, j;

    gBgPaletteSource = (u16 *)buffer;
    for (i = 0; i <= 12; i++) {
        u16 *src = ((u16(*)[16])gBgPaletteBuffer)[i];
        u16 *dst = buffer[i];
        for (j = 0; j < 16; j++) {
            *dst = Gray(*src);
            src++;
            dst++;
        }
    }
    for (i = 13; i < 16; i++) {
        CpuCopy32(((u16(*)[16])gBgPaletteBuffer)[i], buffer[i], 32);
    }
    if (gBackdropOverride) {
        gBackdropColor = Gray((s16)gBackdropColor);
    }
}
