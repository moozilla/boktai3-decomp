#include "global.h"

struct PaletteFade {
    u8 unk_00[0x24];
    u16 *targets;
    s32 deltaR[16][16];
    s32 deltaG[16][16];
    s32 deltaB[16][16];
    s32 red[16][16];
    s32 green[16][16];
    s32 blue[16][16];
    s32 remainingFrames;
    s32 unk_182C;
    s32 duration;
    s32 slot;
    s32 target;
    s32 remainingTargets;
    u32 flags;
};
extern u16 gUnk_03004FC0[16][16];
s32 Div(s32, s32);

#ifdef NONMATCHING
// fuzzy: 86.2%
// scope shape 20 (differences once low registers are set aside)
void sub_08111578(struct PaletteFade *p)
{
    s32 fromOffset = p->slot * 16;
    u16 *fromBase = &gUnk_03004FC0[0][0];
    u16 *from;
    u16 *targets = p->targets;
    u16 *to = targets + p->target * 16;
    s32 i;
    p->remainingFrames = p->duration;
    for (i = 0; i < 16; i++) {
        u32 t;
        u32 f;
        u16 tr, fr, sr;
        p->deltaR[p->slot][i] =
            (t = to[i], tr = 31, tr &= t, f = *(from = (u16 *)(fromBase + fromOffset)), fr = 31,
             fr &= f, (tr - fr) << 24);
        p->deltaG[p->slot][i] = ((((t << 16) >> 21) & 31) - (((f << 16) >> 21) & 31)) << 24;
        p->deltaB[p->slot][i] = ((((t << 16) >> 26) & 31) - (((f << 16) >> 26) & 31)) << 24;
        p->deltaR[p->slot][i] = Div(p->deltaR[p->slot][i], p->duration);
        p->deltaG[p->slot][i] = Div(p->deltaG[p->slot][i], p->duration);
        p->deltaB[p->slot][i] = Div(p->deltaB[p->slot][i], p->duration);
        p->red[p->slot][i] = (f = *from, sr = 31, sr &= f, sr << 24);
        p->green[p->slot][i] = (((f << 16) >> 21) & 31) << 24;
        p->blue[p->slot][i] = (((f << 16) >> 26) & 31) << 24;
        fromOffset++;
    }
    if (p->remainingTargets >= 0) {
        p->remainingTargets--;
        p->target++;
        p->flags |= 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08111578);
#endif
