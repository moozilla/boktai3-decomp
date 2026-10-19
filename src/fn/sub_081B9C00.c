#define gBgPaletteShadow gUnk_03004FC0
#include "global.h"

struct Registry2388PaletteController {
    u8 pad0[0x1c];
    u8 *palettes;
    u8 pad20[0x508];
    u16 flags;
};
extern u8 gBgPaletteShadow[];
void Registry2388_BlendPaletteBanks(u8 *, u8 *, u8 *, int, int, int);

void Registry2388_ApplyPalettePhase(struct Registry2388PaletteController *p, u32 phase)
{
    u8 *base = p->palettes;
    u8 *extra;
    u32 flags = p->flags;
    u32 mask = 0x80;
    if (flags & mask)
        extra = base + 0x120;
    else
        extra = 0;
    switch (phase) {
        case 0:
            Registry2388_BlendPaletteBanks(gBgPaletteShadow, base, base + 0xc0, 0x30, 6, 3);
            if (extra)
                Registry2388_BlendPaletteBanks(gBgPaletteShadow + 0x40, base + 0x40, extra, 0x30, 6,
                                               1);
            break;
        case 1:
            Registry2388_BlendPaletteBanks(gBgPaletteShadow, base, base + 0xc0, 0x10, 6, 3);
            if (extra)
                Registry2388_BlendPaletteBanks(gBgPaletteShadow + 0x40, base + 0x40, extra, 0x10, 6,
                                               1);
            break;
        case 2:
            CpuSet(base, gBgPaletteShadow, 0x04000018);
            break;
        case 3:
            Registry2388_BlendPaletteBanks(gBgPaletteShadow, base, base + 0x60, 0x10, 6, 3);
            break;
        case 4:
            Registry2388_BlendPaletteBanks(gBgPaletteShadow, base + 0x60, base + 0xc0, 0x30, 6, 3);
            if (extra)
                Registry2388_BlendPaletteBanks(gBgPaletteShadow + 0x40, base + 0xa0, extra, 0x30, 6,
                                               1);
            break;
        case 5:
            CpuSet(base + 0xc0, gBgPaletteShadow, 0x04000018);
            if (extra)
                CpuSet(base + 0x120, gBgPaletteShadow + 0x40, 0x04000008);
            break;
    }
}
