#include "bosses/sabata/scene_volume.h"
#include "engine/video/render.h"
// confidence=low: controls the actor palette fade and repeating pulse sequence
void Palette_InterpolateBanks(u16 *, u16 *, u16 *, s32, s32);
void SceneVolume_ResetPendingSequence(struct SceneVolumeActor *);
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

void BossSlot2_UpdateSequencePalette(struct SceneVolumeActor *p)
{
    switch (p->sequenceMode) {
        case 1: {
            u16 *from;
            u16 *to;

            if (p->sequenceTimer < 24) {
                p->sequenceTimer++;
                GetFadeBanks(&from, &to, 0x2E80, 0x680);
                Palette_InterpolateBanks(p->palette, from, to,
                                         Fraction(p->sequenceTimer, 1 << p->sequenceShift),
                                         p->sequenceShift);
            } else {
                SceneVolume_ResetPendingSequence(p);
            }
            break;
        }
        case 2: {
            u16 *from;
            u16 *to;

            if (p->sequenceTimer < (1 << p->sequenceShift)) {
                p->sequenceTimer++;
                GetFadeBanks(&from, &to, 0x680, 0x2DE0);
                Palette_InterpolateBanks(p->palette, from, to,
                                         Fraction(p->sequenceTimer, 1 << p->sequenceShift),
                                         p->sequenceShift);
            } else {
                SceneVolume_ResetPendingSequence(p);
            }
            break;
        }
        case 3: {
            u16 *from;
            u16 *to;

            if (p->sequenceTimer < (1 << p->sequenceShift)) {
                p->sequenceTimer++;
                GetFadeBanks(&from, &to, 0x2DE0, 0x680);
                Palette_InterpolateBanks(p->palette, from, to,
                                         Fraction(p->sequenceTimer, 1 << p->sequenceShift),
                                         p->sequenceShift);
            } else {
                SceneVolume_ResetPendingSequence(p);
            }
            break;
        }
        case 4: {
            s32 half, time, shift;
            p->sequenceTimer = (p->sequenceTimer + 1) & ((1 << p->sequenceShift) - 1);
            time = p->sequenceTimer;
            shift = p->sequenceShift - 1;
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
