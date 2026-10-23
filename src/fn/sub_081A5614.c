#include "global.h"

#ifdef NONMATCHING
// fuzzy: 82.3%
// fix.py best version (reprinted C)
// confidence=low: blends the boss palette banks and stage colors.

// confidence=low: palette transitions and stage colors follow adjacent Vanargand routines.
// MATCHING_CONCESSION: register-pinning

// Remaining differences: interpolation temporaries, spill placement and argument setup.

// confidence=low: palette ownership follows adjacent Vanargand transition routines.
struct PaletteTransition {
    u8 pad0[0x718];
    u8 output[0x200];
    u8 first[0x200];
    u8 second[0x200];
    u8 third[0x200];
    s32 frame;
    u32 selector;
    u16 mode;
    u16 shift;
    u32 alternate;
    u8 padF28[0xc];
    s32 stage;
};
extern u8 *gUnk_030042E4;
extern u16 gUnk_03004FC0[];
void Vanargand_BlendPaletteBanks(void *, void *, void *, s32, s32, s32);
void Vanargand_ResetPaletteTransition(struct PaletteTransition *);
void CpuSet(const void *, void *, u32);

static inline s32 Interpolate(s32 start, s32 end, s32 frame, s32 duration)
{
    s32 value;
    if (frame <= 0)
        return start;
    if (frame >= duration)
        return end;
    value = ((end - start) << 8) * frame / duration;
    if (value >= 0)
        value >>= 8;
    else
        value = -(-value >> 8);
    return start + value;
}

static inline void SetStageColors(s32 stage)
{
    s32 red, green, blue;
    u16 color;
    red = Interpolate(7, 19, stage, 3);
    green = Interpolate(0, 18, stage, 3);
    blue = Interpolate(0, 19, stage, 3);
    color = red | (green << 5) | (blue << 10);
    gUnk_03004FC0[0x56 / 2] = color;
    gUnk_03004FC0[0x10c / 2] = color;
    red = Interpolate(21, 24, stage, 3);
    green = Interpolate(0, 29, stage, 3);
    blue = Interpolate(0, 31, stage, 3);
    color = red | (green << 5) | (blue << 10);
    gUnk_03004FC0[0x5a / 2] = color;
    gUnk_03004FC0[0x10e / 2] = color;
}

static inline void RestorePalette(struct PaletteTransition *p)
{
    u32 alternate = p->alternate;
    s32 selector = 0x6f;
    if (!alternate)
        selector = 0x7f;
    switch (selector) {
        case 0x6f:
            CpuSet(gUnk_030042E4 + 0xde0, p->output, 0x4000020);
            CpuSet(p->first, gUnk_03004FC0, 0x4000068);
            SetStageColors(p->stage);
            break;
        case 0x82:
            CpuSet(gUnk_030042E4 + 0x1040, p->output, 0x4000020);
            CpuSet(p->second, gUnk_03004FC0, 0x4000068);
            break;
        case 0x7f:
            CpuSet(gUnk_030042E4 + 0xfe0, p->output, 0x4000018);
            CpuSet(p->third, gUnk_03004FC0, 0x4000068);
            break;
        default:
            CpuSet(gUnk_030042E4 + 0x3240, p->output, 0x4000008);
            break;
    }
    Vanargand_ResetPaletteTransition(p);
}

static inline void BlendStageColors(s32 stage, s32 frame, s32 duration)
{
    s32 red;
    s32 green;
    s32 blue;
    s32 r0;
    s32 r1;
    s32 g0;
    s32 g1;
    s32 b0;
    s32 b1;
    u16 color;
    typeof(red) c1;
    r0 = Interpolate(7, 19, stage, 3);
    r1 = Interpolate(7, 19, stage + 1, 3);
    g0 = Interpolate(0, 18, stage, 3);
    g1 = Interpolate(0, 18, stage + 1, 3);
    b0 = Interpolate(0, 19, stage, 3);
    b1 = Interpolate(0, 19, stage + 1, 3);
    red = Interpolate(r0, r1, frame, duration);
    c1 = red;
    green = Interpolate(g0, g1, frame, duration);
    blue = Interpolate(b0, b1, frame, duration);
    color = (red | (green << 5)) | (blue << 10);
    gUnk_03004FC0[0x56 / 2] = color;
    gUnk_03004FC0[0x10c / 2] = color;
    r0 = Interpolate(21, 24, stage, 3);
    r1 = Interpolate(21, 24, stage + 1, 3);
    g0 = Interpolate(0, 29, stage, 3);
    g1 = Interpolate(0, 29, stage + 1, 3);
    b0 = Interpolate(0, 31, stage, 3);
    b1 = Interpolate(0, 31, stage + 1, 3);
    red = Interpolate(r0, r1, frame, duration);
    green = Interpolate(g0, g1, frame, duration);
    blue = Interpolate(b0, b1, frame, duration);
    color = (c1 | (green << 5)) | (blue << 10);
    gUnk_03004FC0[0x5a / 2] = color;
    gUnk_03004FC0[0x10e / 2] = color;
}

void Vanargand_UpdatePaletteTransition(struct PaletteTransition *p)
{
    u32 alternate = p->alternate;
    register s32 selector asm("r8") = 0x6f;
    if (!alternate)
        selector = 0x7f;
    switch (p->mode) {
        case 1:
            if (p->frame < (1 << p->shift)) {
                p->frame++;
                Vanargand_BlendPaletteBanks(p->output, gUnk_030042E4 + (selector * 32),
                                            gUnk_030042E4 + 0x1040, p->frame, p->shift, 4);
                Vanargand_BlendPaletteBanks(gUnk_03004FC0, p->first, p->second, p->frame, p->shift,
                                            13);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 2:
            if (p->frame < (1 << p->shift)) {
                p->frame++;
                Vanargand_BlendPaletteBanks(p->output, gUnk_030042E4 + (selector * 32),
                                            gUnk_030042E4 + 0xf60, p->frame, p->shift, 1);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 3:
            if (p->frame < (1 << p->shift)) {
                p->frame++;
                Vanargand_BlendPaletteBanks(p->output, gUnk_030042E4 + (selector * 32),
                                            gUnk_030042E4 + 0xf80, p->frame, p->shift, 1);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 4:
            if (p->frame < (1 << p->shift)) {
                p->frame++;
                Vanargand_BlendPaletteBanks(p->output, gUnk_030042E4 + 0xf60,
                                            gUnk_030042E4 + (selector * 32), p->frame, p->shift, 1);
            } else {
                RestorePalette(p);
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 5:
            if (p->frame < (1 << p->shift)) {
                p->frame++;
                Vanargand_BlendPaletteBanks(p->output, gUnk_030042E4 + 0xf80,
                                            gUnk_030042E4 + (selector * 32), p->frame, p->shift, 1);
            } else {
                RestorePalette(p);
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 6:
            if (p->frame < (1 << p->shift)) {
                p->frame++;
                BlendStageColors(p->stage, p->frame, 1 << p->shift);
            } else {
                SetStageColors(p->stage + 1);
                Vanargand_ResetPaletteTransition(p);
            }
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Vanargand_UpdatePaletteTransition);
#endif
