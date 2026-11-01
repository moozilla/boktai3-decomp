#include "global.h"
struct AxePalette {
    u16 colors[16];
};
struct AxePalettePair {
    u16 colors[32];
};
struct AxePaletteData {
    u8 pad[0x888];
    struct AxePalette black;
    struct AxePalettePair red, white;
};
struct AxePaletteDrawable {
    u8 pad[0x38];
    const u16 *palette;
};
struct AxePaletteActor {
    u32 unused;
    struct AxePaletteDrawable *drawable;
    u8 pad[0x3d0 - 8];
    struct AxePaletteData *data;
};
void Axe_InitEffectPalettes(struct AxePaletteActor *s)
{
    struct AxePaletteData *d = s->data;
    struct AxePaletteDrawable *drawable = s->drawable;
    s32 i = 0;
    const u16 *src = drawable->palette;
    struct AxePalettePair *red = &d->red;
    struct AxePalette *black = &d->black;
    u32 zero = 0;
    u32 full = 0x7fff;
    struct AxePalettePair *white = &d->white;

loop: {
    u32 color;
    black->colors[0] = zero;
    color = 0x1f;
    red->colors[0] = color;
    color = 0x7c00;
    red->colors[16] = color;
    white->colors[0] = full;
    white->colors[16] = *src++;
    red = (void *)((u8 *)red + 2);
    black = (void *)((u8 *)black + 2);
    white = (void *)((u8 *)white + 2);
}
    if (++i < 16)
        goto loop;
}
