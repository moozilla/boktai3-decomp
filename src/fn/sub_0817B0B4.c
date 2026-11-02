#include "global.h"
#include "render.h"
// confidence=low: controls the actor palette fade and repeating pulse sequence
struct PaletteActor {
    u8 pad0[0x504];
    u8 timer, pad505, mode, shift;
    u8 pad508[0x6F8];
    u16 palette[16];
};
void Palette_InterpolateBanks(u16 *, u16 *, u16 *, s32, s32);
void SceneVolume_ResetPendingSequence(void *);
static inline s32 Fraction(s32 time, s32 end)
{
    s32 value;
    if (time <= 0)
        return 0;
    if (time >= end)
        return 24;
    {
        s32 scale = 6144;
        value = (scale * time) / end;
    }
    if (value >= 0)
        return value >> 8;
    return -(-value >> 8);
}

static inline void GetFadeBanks(u16 **from, u16 **to, u32 fromOffset, u32 toOffset)
{
    u8 *banks = gPaletteBankData;

    *from = (u16 *)(banks + fromOffset);
    *to = (u16 *)(banks + toOffset);
}

void BossSlot2_UpdateSequencePalette(struct PaletteActor *p)
{
    switch (p->mode) {
        case 1: {
            u16 *from;
            u16 *to;

            if ((s8)p->timer < 24) {
                p->timer++;
                GetFadeBanks(&from, &to, 0x2E80, 0x680);
                Palette_InterpolateBanks(p->palette, from, to,
                                         Fraction((s8)p->timer, 1 << p->shift), p->shift);
            } else {
                SceneVolume_ResetPendingSequence(p);
            }
            break;
        }
        case 2: {
            u16 *from;
            u16 *to;

            if ((s8)p->timer < (1 << p->shift)) {
                p->timer++;
                GetFadeBanks(&from, &to, 0x680, 0x2DE0);
                Palette_InterpolateBanks(p->palette, from, to,
                                         Fraction((s8)p->timer, 1 << p->shift), p->shift);
            } else {
                SceneVolume_ResetPendingSequence(p);
            }
            break;
        }
        case 3: {
            u16 *from;
            u16 *to;

            if ((s8)p->timer < (1 << p->shift)) {
                p->timer++;
                GetFadeBanks(&from, &to, 0x2DE0, 0x680);
                Palette_InterpolateBanks(p->palette, from, to,
                                         Fraction((s8)p->timer, 1 << p->shift), p->shift);
            } else {
                SceneVolume_ResetPendingSequence(p);
            }
            break;
        }
        case 4: {
            s32 half, time, shift;
            p->timer = (p->timer + 1) & ((1 << p->shift) - 1);
            time = (s8)p->timer;
            shift = p->shift - 1;
            half = 1 << shift;
            if (time < half) {
                u16 *from, *to;
                GetFadeBanks(&from, &to, 0x680, 0x2DE0);
                Palette_InterpolateBanks(p->palette, from, to, time, shift);
            } else {
                u16 *from, *to;
                GetFadeBanks(&from, &to, 0x2DE0, 0x680);
                Palette_InterpolateBanks(p->palette, from, to, time - half, shift);
            }
            break;
        }
    }
}
