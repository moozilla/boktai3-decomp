#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 54.6%
// Open: the colour channels: the original re-zero-extends the colours and
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
extern struct PaletteFade *gUnk_020001D8;
extern u16 gUnk_03004FC0[];
void sub_0811153C(struct PaletteFade *, s32 slot, s32 target, s32 rate, s32 frames);
void sub_08111410(struct PaletteFade *);
void sub_08111578(struct PaletteFade *);
/* Registry routine 734C: fades palette slot -t (default 3) of the palette
   buffer towards palette -g of the fade task over -f frames (default 60; 0 =
   at once), -r (default 8). Per color it keeps the start channels and the
   channel deltas as 8.24 fixed point. */
void PaletteFade_StartFromScript(void)
{
    struct PaletteFade *p;
    s32 slot, target, rate, frames;
    if (!gUnk_020001D8)
        return;
    slot = Script_GetKeywordValue('t', 3);
    target = Script_GetKeywordValue('g', 0);
    rate = Script_GetKeywordValue('r', 8);
    frames = Script_GetKeywordValue('f', 60);
    p = gUnk_020001D8;
    sub_0811153C(p, slot, target, rate, frames);
    if (frames <= 0) {
        u16 *from = &gUnk_03004FC0[p->slot * 16];
        u16 *to = &p->targets[p->target * 16];
        s32 i;
        for (i = 0; i <= 15; i++) {
            u16 t = *to, f = *from;
            s32 fr, fg, fb, tg, tb;
            fr = f & 0x1F;
            p->deltaR[p->slot][i] = ((t & 0x1F) - fr) << 24;
            tg = (u32)(t << 16) >> 21 & 0x1F;
            fg = (u32)(f << 16) >> 21 & 0x1F;
            p->deltaG[p->slot][i] = (tg - fg) << 24;
            tb = (u32)(t << 16) >> 26 & 0x1F;
            fb = (u32)(f << 16) >> 26 & 0x1F;
            p->deltaB[p->slot][i] = (tb - fb) << 24;
            p->r[p->slot][i] = fr << 24;
            p->g[p->slot][i] = fg << 24;
            p->b[p->slot][i] = fb << 24;
            from++;
            to++;
        }
        sub_08111410(p);
    } else
        sub_08111578(p);
}
#else
INCLUDE_ASM("asm/nonmatching", PaletteFade_StartFromScript);
#endif
