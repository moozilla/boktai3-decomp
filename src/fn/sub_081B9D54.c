#include "global.h"

struct Registry2388PaletteController {
    u8 pad0[0x1c];
    u8 *palettes;
    u8 pad20[0x500];
    s16 elapsed;
    u8 shift;
    u8 mode;
    u8 pad524[4];
    u16 flags;
};
extern u8 gBgPaletteBuffer[];
void Registry2388_BlendPaletteBanks(u8 *, u8 *, u8 *, int, int, int);

void Registry2388_BlendTimedPalettes(struct Registry2388PaletteController *p)
{
    u8 *base = p->palettes;
    u8 *extra;
    u32 flags = p->flags;
    u32 mask = 0x80;
    if (flags & mask)
        extra = base + 0x120;
    else
        extra = 0;
    switch (p->mode) {
        case 0:
        case 2:
            if (!extra)
                extra = base + 0x100;
            Registry2388_BlendPaletteBanks(gBgPaletteBuffer, base + 0x60, base + 0xc0, p->elapsed,
                                           p->shift, 2);
            Registry2388_BlendPaletteBanks(gBgPaletteBuffer + 0x40, base + 0xa0, extra, p->elapsed,
                                           p->shift, 1);
            break;
        case 1:
        case 3:
            if (!extra)
                extra = base + 0x100;
            Registry2388_BlendPaletteBanks(gBgPaletteBuffer, base + 0xc0, base, p->elapsed,
                                           p->shift, 2);
            Registry2388_BlendPaletteBanks(gBgPaletteBuffer + 0x40, extra, base + 0x40, p->elapsed,
                                           p->shift, 1);
            break;
    }
}
