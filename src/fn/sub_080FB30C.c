#include "global.h"

static inline s32 ClampLevel(s32 level)
{
    if (level > 3)
        return 3;
    if (level < 0)
        return 0;
    return level;
}

struct AxeFlashState {
    u8 unk_00[0x908];
    u16 palette[16];
    u8 unk_928[0xE4C - 0x928];
    u32 flags;
    u8 unk_E50[0xECE - 0xE50];
    u8 timer;
    s8 level;
    u8 start;
    u8 stop;
    u8 unk_ED2[4];
    u8 reset;
};
struct AxeFlashDrawable {
    u8 unk_00[0x2C];
    u8 animData[6];
    u16 profile;
    u8 unk_34[4];
    u16 *palette;
};
struct AxeFlashActor {
    u32 unk_00;
    struct AxeFlashDrawable *drawable;
    u8 unk_08[0x1E0 - 8];
    u16 paletteBank;
    u8 unk_1E2[4];
    u16 paletteOffset;
    u8 unk_1E8[0x3D0 - 0x1E8];
    struct AxeFlashState *state;
};
void AnimData_SelectPaletteBank(void *, u32);

static inline u32 TestFlag(u32 flags, u32 mask)
{
    if (flags & mask)
        return 1;
    return 0;
}

#ifdef NONMATCHING
// fuzzy: 78.6%
// scope shape 31 (differences once low registers are set aside)
// confidence=low: axe flash actor update; fades the palette by level and timer, then clears the active flag.
void sub_080FB30C(struct AxeFlashActor *actor)
{
    struct AxeFlashState *state = actor->state;
    u32 *flags;
    if (state->start) {
        state->level = 0;
        state->timer = 0;
        {
            u32 bit = 0x80;
            state->flags |= bit;
        }
        state->start = 0;
    }
    flags = &state->flags;
    if (state->stop) {
        {
            u32 bit = 0x100;
            *flags |= bit;
        }
        *flags &= ~0x80;
        state->level = 3;
        state->timer = 0;
        state->stop = 0;
    }
    if (state->reset) {
        {
            u32 bit = 0x100;
            *flags |= bit;
        }
        *flags &= ~0x80;
        {
            s32 lv = state->level;
            if (lv > 3)
                lv = 3;
            else if (lv < 0)
                lv = 0;
            state->level = lv;
        }
        state->timer = 0;
        state->reset = 0;
    }
    {
        u32 mask = 0x180;
        if (TestFlag(*flags, mask)) {
            actor->drawable->profile = 0x13F;
            actor->drawable->palette = state->palette;
            switch (state->level) {
                case 0:
                    state->palette[1] = 0;
                    break;
                case 1:
                    state->palette[1] = 16;
                    break;
                case 2:
                    state->palette[1] = 24;
                    break;
                case 3:
                    state->palette[1] = 31;
                    break;
            }
            if (++state->timer > 9) {
                if (!TestFlag(*flags, 0x80)) {
                    if (--state->level <= 0) {
                        AnimData_SelectPaletteBank(actor->drawable->animData,
                                                   actor->paletteBank + actor->paletteOffset);
                        {
                            u32 clr = ~0x100;
                            *flags &= clr;
                        }
                        return;
                    }
                } else {
                    if (state->level < 3)
                        state->level++;
                }
                state->timer = 0;
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080FB30C);
#endif
