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
static inline struct PaletteFade *PaletteFade_GetOwner(struct PaletteFade **owner)
{
    return *owner;
}

union PaletteChannelValue {
    u32 full;
    u16 value;
};
#define PALETTE_FADE_CHANNEL(color, shift, result, channelMask)                                    \
    ((result).full = 0, (result).value = ((color) << 16) >> (shift) & (channelMask), (result).full)

void sub_08111578(struct PaletteFade *p)
{
    struct PaletteFade *owner = p;
    s32 fromOffset = p->slot * 32;
    u8 *fromBase = (u8 *)gBgPaletteBuffer;
    u16 *from;
    u16 *to = p->targets;
    s32 i;
    to += p->target * 16;
    p->remainingFrames = p->duration;
    for (i = 0; i < 16; i++) {
        u32 t;
        union PaletteChannelValue targetChannel, sourceChannel, currentChannel;
        u32 f;
        u32 current;
        u16 tr, fr, sr;
        p->deltaR[p->slot][i] =
            (t = to[i], tr = 31, tr &= t, f = *(from = (u16 *)(fromBase + (fromOffset + i * 2))),
             fr = 31, fr &= f, (tr - fr) << 24);
        {
            const u16 mask[1] = {31};
            PaletteFade_GetOwner(&owner)->deltaG[p->slot][i] =
                (PALETTE_FADE_CHANNEL(t, 21, targetChannel, mask[0]),
                 PALETTE_FADE_CHANNEL(f, 21, sourceChannel, mask[0]),
                 targetChannel.full - sourceChannel.full)
                << 24;
            p->deltaB[p->slot][i] = (PALETTE_FADE_CHANNEL(t, 26, targetChannel, mask[0]),
                                     PALETTE_FADE_CHANNEL(f, 26, sourceChannel, mask[0]),
                                     targetChannel.full - sourceChannel.full)
                                    << 24;
            p->deltaR[p->slot][i] = Div(p->deltaR[p->slot][i], p->duration);
            PaletteFade_GetOwner(&owner)->deltaG[p->slot][i] =
                Div(PaletteFade_GetOwner(&owner)->deltaG[p->slot][i], p->duration);
            p->deltaB[p->slot][i] = Div(p->deltaB[p->slot][i], p->duration);
            p->red[p->slot][i] = (current = *from, sr = 31, sr &= current, sr << 24);
            p->green[p->slot][i] = PALETTE_FADE_CHANNEL(current, 21, currentChannel, mask[0]) << 24;
            p->blue[p->slot][i] = PALETTE_FADE_CHANNEL(current, 26, currentChannel, mask[0]) << 24;
        }
    }
    if (p->remainingTargets >= 0) {
        p->remainingTargets--;
        p->target++;
        p->flags |= 1;
    }
}
