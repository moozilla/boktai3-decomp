#include "global.h"
#include "render.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: controls the actor palette fade and repeating pulse sequence
struct PaletteActor {
    u8 pad0[0x504];
    u8 timer, pad505, mode, shift;
    u8 pad508[0x6F8];
    u16 palette[16];
};
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
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
void BossSlot2_UpdateSequencePalette(struct PaletteActor *p)
{
    switch (p->mode) {
        case 1:
            if ((s8)p->timer < 24) {
                void *a;
                register void *b asm("r6");
                register u8 *base asm("r0");
                p->timer++;
                base = gPaletteBankData;
                a = base + 0x2E80;
                b = base + 0x680;
                Palette_InterpolateBanks(p->palette, a, b, Fraction((s8)p->timer, 1 << p->shift),
                                         p->shift);
            } else
                SceneVolume_ResetPendingSequence(p);
            break;
        case 2:
            if ((s8)p->timer < (1 << p->shift)) {
                void *a;
                register void *b asm("r7");
                p->timer++;
                a = gPaletteBankData + 0x680;
                b = gPaletteBankData + 0x2DE0;
                Palette_InterpolateBanks(p->palette, a, b, Fraction((s8)p->timer, 1 << p->shift),
                                         p->shift);
            } else
                SceneVolume_ResetPendingSequence(p);
            break;
        case 3:
            if ((s8)p->timer < (1 << p->shift)) {
                void *a;
                register void *b asm("r7");
                p->timer++;
                a = gPaletteBankData + 0x2DE0;
                b = gPaletteBankData + 0x680;
                Palette_InterpolateBanks(p->palette, a, b, Fraction((s8)p->timer, 1 << p->shift),
                                         p->shift);
            } else
                SceneVolume_ResetPendingSequence(p);
            break;
        case 4: {
            s32 half, time, shift;
            p->timer = (p->timer + 1) & ((1 << p->shift) - 1);
            time = (s8)p->timer;
            shift = p->shift - 1;
            half = 1 << shift;
            if (time < half) {
                void *a = gPaletteBankData + 0x680;
                void *b = gPaletteBankData + 0x2DE0;
                Palette_InterpolateBanks(p->palette, a, b, time, shift);
            } else {
                void *a = gPaletteBankData + 0x2DE0;
                void *b = gPaletteBankData + 0x680;
                Palette_InterpolateBanks(p->palette, a, b, time - half, shift);
            }
            break;
        }
    }
}
