#include "global.h"
#include "render.h"

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
s32 Div(s32, s32);

#ifdef NONMATCHING
// fuzzy: 93.9%
void sub_08111578(struct PaletteFade *p)
{
    s32 fromOffset = p->slot * 32;
    u8 *fromBase = (u8 *)gBgPaletteBuffer;
    u16 *from;
    u16 *to = p->targets;
    s32 i;
    to += p->target * 16;
    p->remainingFrames = p->duration;
    for (i = 0; i < 16; i++) {
        u32 t;
        u32 mask;
        u32 f;
        u16 tr, fr, sr;
        p->deltaR[p->slot][i] =
            (t = to[i], tr = 31, tr &= t, f = *(from = (u16 *)(fromBase + (fromOffset + i * 2))),
             fr = 31, fr &= f, (tr - fr) << 24);
        mask = 31;
        p->deltaG[p->slot][i] = ((((t << 16) >> 21) & mask) - (((f << 16) >> 21) & mask)) << 24;
        p->deltaB[p->slot][i] = ((((t << 16) >> 26) & mask) - (((f << 16) >> 26) & mask)) << 24;
        p->deltaR[p->slot][i] = Div(p->deltaR[p->slot][i], p->duration);
        p->deltaG[p->slot][i] = Div(p->deltaG[p->slot][i], p->duration);
        p->deltaB[p->slot][i] = Div(p->deltaB[p->slot][i], p->duration);
        p->red[p->slot][i] = (f = *from, sr = 31, sr &= f, sr << 24);
        p->green[p->slot][i] = (((f << 16) >> 21) & mask) << 24;
        p->blue[p->slot][i] = (((f << 16) >> 26) & mask) << 24;
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
