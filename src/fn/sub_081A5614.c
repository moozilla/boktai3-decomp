#include "global.h"
#include "render.h"

// Tentative: palette ownership follows adjacent Vanargand transition routines.
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
    {
        s32 red, green, blue;
        u16 nextColor;
        red = Interpolate(21, 24, stage, 3);
        green = Interpolate(0, 29, stage, 3);
        blue = Interpolate(0, 31, stage, 3);
        nextColor = red | (green << 5) | (blue << 10);
        gUnk_03004FC0[0x5a / 2] = nextColor;
        gUnk_03004FC0[0x10e / 2] = nextColor;
    }
}

static inline void RestorePalette(struct PaletteTransition *p)
{
    s32 selector = 0x6f;
    u32 alternate = p->alternate;
    if (!alternate)
        selector = 0x7f;
    switch (selector) {
        case 0x6f:
            CpuSet(gPaletteBankData + 0xde0, p->output, 0x4000020);
            CpuSet(p->first, gUnk_03004FC0, 0x4000068);
            SetStageColors(p->stage);
            break;
        case 0x82:
            CpuSet(gPaletteBankData + 0x1040, p->output, 0x4000020);
            CpuSet(p->second, gUnk_03004FC0, 0x4000068);
            break;
        case 0x7f:
            CpuSet(gPaletteBankData + 0xfe0, p->output, 0x4000018);
            CpuSet(p->third, gUnk_03004FC0, 0x4000068);
            break;
        default:
            CpuSet(gPaletteBankData + 0x3240, p->output, 0x4000008);
            break;
    }
    Vanargand_ResetPaletteTransition(p);
}

static inline void BlendStageColors(s32 frame, s32 stage, s32 duration)
{
    s32 red;
    s32 green;
    s32 blue;
    s32 startRed;
    s32 endRed;
    s32 startGreen;
    s32 endGreen;
    s32 startBlue;
    s32 endBlue;
    u16 color;
    s32 nextRed, nextGreen, nextBlue;
    startRed = Interpolate(7, 19, stage, 3);
    endRed = Interpolate(7, 19, stage + 1, 3);
    startGreen = Interpolate(0, 18, stage, 3);
    endGreen = Interpolate(0, 18, stage + 1, 3);
    startBlue = Interpolate(0, 19, stage, 3);
    endBlue = Interpolate(0, 19, stage + 1, 3);
    red = Interpolate(startRed, endRed, frame, duration);
    green = Interpolate(startGreen, endGreen, frame, duration);
    blue = Interpolate(startBlue, endBlue, frame, duration);
    color = (red | (green << 5)) | (blue << 10);
    gUnk_03004FC0[0x56 / 2] = color;
    gUnk_03004FC0[0x10c / 2] = color;
    startRed = Interpolate(21, 24, stage, 3);
    endRed = Interpolate(21, 24, stage + 1, 3);
    startGreen = Interpolate(0, 29, stage, 3);
    endGreen = Interpolate(0, 29, stage + 1, 3);
    startBlue = Interpolate(0, 31, stage, 3);
    endBlue = Interpolate(0, 31, stage + 1, 3);
    nextRed = Interpolate(startRed, endRed, frame, duration);
    nextGreen = Interpolate(startGreen, endGreen, frame, duration);
    nextBlue = Interpolate(startBlue, endBlue, frame, duration);
    color = (nextRed | (nextGreen << 5)) | (nextBlue << 10);
    gUnk_03004FC0[0x5a / 2] = color;
    gUnk_03004FC0[0x10e / 2] = color;
}

void Vanargand_UpdatePaletteTransition(struct PaletteTransition *p)
{
    u32 alternate = p->alternate;
    s32 selector = 0x6f;
    if (!alternate)
        selector = 0x7f;
    switch (p->mode) {
        case 1:
            if (p->frame < (1 << p->shift)) {
                u8 *source;
                u8 *target;
                p->frame++;
                source = gPaletteBankData + (selector * 32);
                target = gPaletteBankData + 0x1040;
                Vanargand_BlendPaletteBanks(p->output, source, target, p->frame, p->shift, 4);
                Vanargand_BlendPaletteBanks(gUnk_03004FC0, p->first, p->second, p->frame, p->shift,
                                            13);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 2:
            if (p->frame < (1 << p->shift)) {
                u8 *source;
                u8 *target;
                p->frame++;
                source = gPaletteBankData + (selector * 32);
                target = gPaletteBankData + 0xf60;
                Vanargand_BlendPaletteBanks(p->output, source, target, p->frame, p->shift, 1);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 3:
            if (p->frame < (1 << p->shift)) {
                u8 *source;
                u8 *target;
                p->frame++;
                source = gPaletteBankData + (selector * 32);
                target = gPaletteBankData + 0xf80;
                Vanargand_BlendPaletteBanks(p->output, source, target, p->frame, p->shift, 1);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 4:
            if (p->frame < (1 << p->shift)) {
                u8 *source;
                u8 *target;
                p->frame++;
                source = gPaletteBankData + 0xf60;
                target = gPaletteBankData + (selector * 32);
                Vanargand_BlendPaletteBanks(p->output, source, target, p->frame, p->shift, 1);
            } else {
                RestorePalette(p);
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 5:
            if (p->frame < (1 << p->shift)) {
                u8 *source;
                u8 *target;
                p->frame++;
                source = gPaletteBankData + 0xf80;
                target = gPaletteBankData + (selector * 32);
                Vanargand_BlendPaletteBanks(p->output, source, target, p->frame, p->shift, 1);
            } else {
                RestorePalette(p);
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 6:
            if (p->frame < (1 << p->shift)) {
                p->frame++;
                BlendStageColors(p->frame, p->stage, 1 << p->shift);
            } else {
                SetStageColors(p->stage + 1);
                Vanargand_ResetPaletteTransition(p);
            }
            break;
    }
}
