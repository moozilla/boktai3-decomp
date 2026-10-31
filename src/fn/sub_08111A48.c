#include "global.h"
#include "engine.h"
#include "ram.h"
#include "render.h"
struct PaletteFade {
    u8 pad0[0x24];
    u16 *targets;
    s32 deltaR[16][16];
    s32 deltaG[16][16];
    s32 deltaB[16][16];
    s32 r[16][16];
    s32 g[16][16];
    s32 b[16][16];
    u8 pad1828[0xC];
    s32 slot, target, rate;
};
void PaletteFade_SetTransitionParameters(struct PaletteFade *, s32 slot, s32 target, s32 rate,
                                         s32 frames);
void PaletteFade_AdvanceColors(struct PaletteFade *);
void sub_08111578(struct PaletteFade *);
/* Registry routine 734C: fades palette slot -t (default 3) of the palette
   buffer towards palette -g of the fade task over -f frames (default 60; 0 =
   at once), -r (default 8). Per color it keeps the start channels and the
   channel deltas as 8.24 fixed point. */
#define PALETTE_FADE_READ_RED(color_, red_, sample_)                                               \
    ((color_ = (sample_)), (red_ = 31), (red_ &= color_), red_)

void PaletteFade_StartFromScript(void)
{
    struct PaletteFade *p;
    s32 slot;
    s32 target;
    s32 rate;
    s32 frames;
    if (!gPaletteFade)
        return;
    slot = Script_GetKeywordValue('t', 3);
    target = Script_GetKeywordValue('g', 0);
    rate = Script_GetKeywordValue('r', 8);
    frames = Script_GetKeywordValue('f', 60);
    p = gPaletteFade;
    PaletteFade_SetTransitionParameters(p, slot, target, rate, frames);
    if (frames <= 0) {
        s32 sourceBankOffset = p->slot * 32;
        u16 *sourcePalette = gBgPaletteBuffer;
        u16 *targetPalette = p->targets;
        s32 targetBankOffset = p->target * 32;
        s32 i;
        for (i = 0; i <= 15; i++) {
            u32 targetColor, sourceColor;
            u16 initialRed, initialGreen, initialBlue;
            u16 targetRed, sourceRed;
            p->deltaR[p->slot][i] =
                (PALETTE_FADE_READ_RED(targetColor, targetRed,
                                       *(u16 *)((u32)targetPalette + i * 2 + targetBankOffset)) -
                 (initialRed = PALETTE_FADE_READ_RED(
                      sourceColor, sourceRed,
                      *(u16 *)((u32)sourcePalette + i * 2 + sourceBankOffset))))
                << 24;
            p->deltaG[p->slot][i] = ((((targetColor <<= 16) >> 21) & 31) -
                                     (initialGreen = ((sourceColor <<= 16) >> 21) & 31))
                                    << 24;
            p->deltaB[p->slot][i] =
                (((targetColor >> 26) & 31) - (initialBlue = (sourceColor >> 26) & 31)) << 24;
            p->r[p->slot][i] = initialRed << 24;
            p->g[p->slot][i] = initialGreen << 24;
            p->b[p->slot][i] = initialBlue << 24;
        }

        PaletteFade_AdvanceColors(p);
    } else
        sub_08111578(p);
}
