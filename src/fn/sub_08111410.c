#include "global.h"
struct PaletteFade {
    u8 unknown0[0x24];
    u16 *targets;
    s32 deltaR[16][16], deltaG[16][16], deltaB[16][16];
    s32 red[16][16], green[16][16], blue[16][16];
    u8 unknown1828[12];
    s32 slot;
};
extern u16 gBgPaletteBuffer[16][16];
void PaletteFade_AdvanceColors(struct PaletteFade *p)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        p->red[p->slot][i] += p->deltaR[p->slot][i];
        p->green[p->slot][i] += p->deltaG[p->slot][i];
        p->blue[p->slot][i] += p->deltaB[p->slot][i];
        gBgPaletteBuffer[p->slot][i] = (p->red[p->slot][i] >> 24) |
                                       ((p->green[p->slot][i] >> 24) << 5) |
                                       ((p->blue[p->slot][i] >> 24) << 10);
    }
}
