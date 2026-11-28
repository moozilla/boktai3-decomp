#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_BlendPaletteBanks(void *, void *, void *, s32, s32, s32);
void CpuSet(const void *, void *, u32);

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
    startRed = Vanargand_InterpolateStageChannel(7, 19, stage, 3);
    endRed = Vanargand_InterpolateStageChannel(7, 19, stage + 1, 3);
    startGreen = Vanargand_InterpolateStageChannel(0, 18, stage, 3);
    endGreen = Vanargand_InterpolateStageChannel(0, 18, stage + 1, 3);
    startBlue = Vanargand_InterpolateStageChannel(0, 19, stage, 3);
    endBlue = Vanargand_InterpolateStageChannel(0, 19, stage + 1, 3);
    red = Vanargand_InterpolateStageChannel(startRed, endRed, frame, duration);
    green = Vanargand_InterpolateStageChannel(startGreen, endGreen, frame, duration);
    blue = Vanargand_InterpolateStageChannel(startBlue, endBlue, frame, duration);
    color = (red | (green << 5)) | (blue << 10);
    gBgPaletteBuffer[0x56 / 2] = color;
    gBgPaletteBuffer[0x10c / 2] = color;
    startRed = Vanargand_InterpolateStageChannel(21, 24, stage, 3);
    endRed = Vanargand_InterpolateStageChannel(21, 24, stage + 1, 3);
    startGreen = Vanargand_InterpolateStageChannel(0, 29, stage, 3);
    endGreen = Vanargand_InterpolateStageChannel(0, 29, stage + 1, 3);
    startBlue = Vanargand_InterpolateStageChannel(0, 31, stage, 3);
    endBlue = Vanargand_InterpolateStageChannel(0, 31, stage + 1, 3);
    nextRed = Vanargand_InterpolateStageChannel(startRed, endRed, frame, duration);
    nextGreen = Vanargand_InterpolateStageChannel(startGreen, endGreen, frame, duration);
    nextBlue = Vanargand_InterpolateStageChannel(startBlue, endBlue, frame, duration);
    color = (nextRed | (nextGreen << 5)) | (nextBlue << 10);
    gBgPaletteBuffer[0x5a / 2] = color;
    gBgPaletteBuffer[0x10e / 2] = color;
}

void Vanargand_UpdatePaletteTransition(struct VanargandPairAttack *p)
{
    u32 alternate = p->variant;
    s32 selector = 0x6f;
    if (!alternate)
        selector = 0x7f;
    switch (p->paletteMode) {
        case 1:
            if (p->paletteFrame < (1 << p->paletteShift)) {
                u8 *source;
                u8 *target;
                p->paletteFrame++;
                source = gPaletteBankData + (selector * 32);
                target = gPaletteBankData + 0x1040;
                Vanargand_BlendPaletteBanks(p->palettes, source, target, p->paletteFrame,
                                            p->paletteShift, 4);
                Vanargand_BlendPaletteBanks(gBgPaletteBuffer, p->fadeA, p->fadeB, p->paletteFrame,
                                            p->paletteShift, 13);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 2:
            if (p->paletteFrame < (1 << p->paletteShift)) {
                u8 *source;
                u8 *target;
                p->paletteFrame++;
                source = gPaletteBankData + (selector * 32);
                target = gPaletteBankData + 0xf60;
                Vanargand_BlendPaletteBanks(p->palettes, source, target, p->paletteFrame,
                                            p->paletteShift, 1);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 3:
            if (p->paletteFrame < (1 << p->paletteShift)) {
                u8 *source;
                u8 *target;
                p->paletteFrame++;
                source = gPaletteBankData + (selector * 32);
                target = gPaletteBankData + 0xf80;
                Vanargand_BlendPaletteBanks(p->palettes, source, target, p->paletteFrame,
                                            p->paletteShift, 1);
            } else {
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 4:
            if (p->paletteFrame < (1 << p->paletteShift)) {
                u8 *source;
                u8 *target;
                p->paletteFrame++;
                source = gPaletteBankData + 0xf60;
                target = gPaletteBankData + (selector * 32);
                Vanargand_BlendPaletteBanks(p->palettes, source, target, p->paletteFrame,
                                            p->paletteShift, 1);
            } else {
                Vanargand_LoadStagePalette(p, p);
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 5:
            if (p->paletteFrame < (1 << p->paletteShift)) {
                u8 *source;
                u8 *target;
                p->paletteFrame++;
                source = gPaletteBankData + 0xf80;
                target = gPaletteBankData + (selector * 32);
                Vanargand_BlendPaletteBanks(p->palettes, source, target, p->paletteFrame,
                                            p->paletteShift, 1);
            } else {
                Vanargand_LoadStagePalette(p, p);
                Vanargand_ResetPaletteTransition(p);
            }
            break;

        case 6:
            if (p->paletteFrame < (1 << p->paletteShift)) {
                p->paletteFrame++;
                BlendStageColors(p->paletteFrame, p->transition, 1 << p->paletteShift);
            } else {
                Vanargand_SetStageColors(p->transition + 1);
                Vanargand_ResetPaletteTransition(p);
            }
            break;
    }
}
