#include "global.h"
#include "engine.h"
#include "ram.h"
#include "render.h"
#ifdef NONMATCHING
// fuzzy: 52.9%
// fix.py best (sweep after the inlined-insn fix)
// Open: the color channels: the original re-zero-extends the colors and
// uses fresh constants for the red channel.
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
inline static u32 Arg1(u32 k_, s32 tg, s32 fg)
{
    return (tg - fg) << k_;
}

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
        u16 *from = &gBgPaletteBuffer[p->slot * 16];
        u16 *to = &p->targets[p->target * 16];
        s32 i;
        for (i = 0; i <= 15; i++) {
            u16 t = *to;
            u16 f = *from;
            s32 fr;
            s32 fg;
            s32 fb;
            s32 tg;
            s32 tb;
            do {
                fr = f & 0x1F;
            } while (0);
            p->deltaR[p->slot][i] = ((t & 0x1F) - fr) << 24;
            tg = (((u32)(t << 16)) >> 21) & 0x1F;
            fg = (((u32)(f << 16)) >> 21) & 0x1F;
            p->deltaG[p->slot][i] = Arg1(24, tg, fg);
            tb = (((u32)(t << 16)) >> 26) & 0x1F;
            fb = (((u32)((unsigned short)(f << 16))) >> 26) & 0x1F;
            p->deltaB[p->slot][i] = (tb - fb) << 24;
            p->r[p->slot][i] = fr << 24;
            p->g[p->slot][i] = fg << 24;
            p->b[p->slot][i] = fb << 24;
            from++;
            to++;
        }

        PaletteFade_AdvanceColors(p);
    } else
        sub_08111578(p);
}

#else
INCLUDE_ASM("asm/nonmatching", PaletteFade_StartFromScript);
#endif
