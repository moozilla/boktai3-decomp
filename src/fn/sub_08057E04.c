// MATCHING_CONCESSION: register-pinning
#include "scene_panel_effects.h"
/* Persistent loop state; empty barriers preserve the reviewed stack layout. */
struct ScenePanelPaletteScratch {
    s32 column;
    const u16 *previous;
    s32 remaining, nextRow;
    u32 rowOffset;
    u16 *paletteBase;
};
void ScenePanel_UpdateTimePalette(struct ScenePanel *input)
{
    register struct ScenePanelEffectsView *panel asm("r8") = (void *)input;
    struct ScenePanelPaletteScratch cache;
    u32 phase;
    u8 *phaseField;
    phase = SunClock_GetPhase();
    phaseField = &panel->timePhase;
    if (*phaseField != phase) {
        *phaseField = phase;
        ScenePanel_SelectTimePalette((struct ScenePanel *)panel, *phaseField);
    }
    if (panel->paletteBlendActive) {
        u16 *blendFrame;
        u32 nextFrame;
        panel->channelsDirty = 1;
        blendFrame = &panel->paletteBlendFrame;
        nextFrame = *blendFrame + 1;
        *blendFrame = nextFrame;
        if ((u16)nextFrame > 63) {
            const u16 *source;
            u32 paletteOffset = panel->targetPalette * 4;
            const u16 **sources = panel->paletteSources;
            u16 *destination;
            s32 i;
            sources = (void *)((u8 *)sources + paletteOffset);
            destination = &panel->paletteColors[0][0];
            source = *sources;
            for (i = 15; i >= 0; i--) {
                CpuSet(source, destination, 0x04000008);
                destination += 16;
                source += 16;
            }
            panel->paletteBlendActive = 0;
            {
                struct ScenePanelMainSprite *sprite = &panel->mainSprite;
                if (ScenePanel_IsSpriteVisible(sprite)) {
                    s32 frame = ScenePanel_AdvancePaletteFrame(sprite);
                    u16 *framePointer = &panel->mainSprite.paletteFrame;
                    *framePointer = frame;
                    {
                        struct ScenePanelGraphics *graphics = &panel->mainSprite.graphics;
                        u32 bank;
                        u32 frameValue;
                        bank = panel->targetPalette * 4;
                        frameValue = *framePointer;
                        frameValue += 0x203;
                        bank += frameValue;
                        AnimData_SelectPaletteBank(graphics, bank);
                    }
                }
            }
        } else {
            s32 newWeight;
            s32 oldWeight;
            register const u16 *previous asm("r12");
            const u16 *target;
            u32 paletteOffset;
            const u16 **sources;
            register s32 row asm("r0");
            {
                s32 sample = *blendFrame;
                s32 inverse = 64 - sample;
                oldWeight = inverse;
                newWeight = sample;
            }
            paletteOffset = panel->previousPalette * 4;
            sources = panel->paletteSources;
            previous = *(const u16 **)((u8 *)sources + paletteOffset);
            paletteOffset = panel->targetPalette * 4;
            sources = (void *)((u8 *)sources + paletteOffset);
            target = *sources;
            row = 0;
            {
                register u16 *base asm("r5") = &panel->paletteColors[0][0];
                cache.paletteBase = base;
            }
            do {
                cache.column = 0;
                {
                    register u32 rowOffset asm("r2") = row * 32;
                    cache.rowOffset = rowOffset;
                }
                cache.nextRow = row + 1;
                do {
                    register const u16 *previousView asm("r5") = previous;
                    u32 startColor = *previousView;
                    s32 redStart = 31;
                    s32 greenStart;
                    s32 redEnd;
                    u32 endColor;
                    s32 greenEnd;
                    register s32 channelMask asm("r5");
                    register s32 red, green asm("r2"), blue asm("r0");
                    redStart &= startColor;
                    startColor <<= 16;
                    greenStart = startColor >> 21;
                    redEnd = 31;
                    greenStart &= redEnd;
                    startColor >>= 26;
                    startColor &= redEnd;
                    endColor = *target;
                    redEnd &= endColor;
                    endColor <<= 16;
                    greenEnd = endColor >> 21;
                    channelMask = 31;
                    greenEnd &= channelMask;
                    endColor >>= 26;
                    endColor &= channelMask;
                    red = (oldWeight * redStart + newWeight * redEnd) >> 6;
                    green = (oldWeight * greenStart + newWeight * greenEnd) >> 6;
                    blue = (oldWeight * (s32)startColor + newWeight * (s32)endColor) >> 6;

                    asm("" : "+m"(cache.column), "+m"(cache.rowOffset), "+m"(cache.paletteBase));
                    {
                        s32 column = cache.column;
                        register u32 offset asm("r1") = column * 2;
                        register u16 *destination asm("r1");
                        asm("" : : "r"(column));
                        offset += cache.rowOffset;
                        destination = (u16 *)((u8 *)cache.paletteBase + offset);
                        blue <<= 10;
                        green <<= 5;
                        blue |= green;
                        blue |= red;
                        *destination = blue;
                    }
                    {
                        register u32 step asm("r5") = 2;
                        asm("" : "+r"(step));
                        previous = (u16 *)((u8 *)previous + step);
                    }
                    target++;
                    asm("" : "+m"(cache.column));
                    cache.column++;
                } while (cache.column <= 15);
                row = cache.nextRow;
            } while (row <= 15);
            {
                struct ScenePanelMainSprite *sprite = &panel->mainSprite;
                if (ScenePanel_IsSpriteVisible(sprite)) {
                    s32 frame = ScenePanel_AdvancePaletteFrame(sprite);
                    u8 **ram = &gPaletteBankData;
                    u32 offset;
                    u8 *ramBase;
                    register const u16 *target asm("r12");
                    u16 *destination;
                    offset = panel->previousPalette * 4;
                    frame += 0x203;
                    offset += frame;
                    offset <<= 5;
                    ramBase = *ram;
                    cache.previous = (u16 *)(ramBase + offset);
                    offset = panel->targetPalette * 4 + frame;
                    offset <<= 5;
                    target = (u16 *)(ramBase + offset);
                    destination = panel->mainSprite.paletteColors;
                    {
                        register s32 remaining asm("r5") = 15;
                        cache.remaining = remaining;
                    }
                    do {
                        register const u16 *previousView asm("r0") = cache.previous;
                        u32 startColor = *previousView;
                        s32 redStart = 31;
                        s32 greenStart;
                        register s32 startMask asm("r1");
                        s32 redEnd;
                        u32 endColor;
                        s32 greenEnd;
                        register s32 channelMask asm("r5");
                        s32 red, green, blue;
                        redStart &= startColor;
                        startColor <<= 16;
                        greenStart = startColor >> 21;
                        startMask = 31;
                        greenStart &= startMask;
                        startColor >>= 26;
                        startColor &= startMask;
                        {
                            register const u16 *targetView asm("r5") = target;
                            endColor = *targetView;
                        }
                        redEnd = 31;
                        redEnd &= endColor;
                        endColor <<= 16;
                        greenEnd = endColor >> 21;
                        channelMask = 31;
                        greenEnd &= channelMask;
                        endColor >>= 26;
                        endColor &= channelMask;
                        red = (oldWeight * redStart + newWeight * redEnd) >> 6;
                        green = (oldWeight * greenStart + newWeight * greenEnd) >> 6;
                        blue = (oldWeight * (s32)startColor + newWeight * (s32)endColor) >> 6;

                        blue <<= 10;
                        green <<= 5;
                        blue |= green;
                        blue |= red;
                        *destination = blue;
                        {
                            register const u16 *next asm("r5") = cache.previous + 1;
                            cache.previous = next;
                        }
                        {
                            u32 step = 2;
                            target = (u16 *)((u8 *)target + step);
                        }
                        destination++;
                        {
                            register s32 remaining asm("r1") = cache.remaining - 1;
                            cache.remaining = remaining;
                            if (remaining < 0)
                                break;
                        }
                    } while (1);
                    panel->mainSprite.graphics.paletteOverride = panel->mainSprite.paletteColors;
                }
            }
        }
    } else {
        struct ScenePanelMainSprite *sprite = &panel->mainSprite;
        if (ScenePanel_IsSpriteVisible(sprite)) {
            s32 frame = ScenePanel_AdvancePaletteFrame(sprite);
            u16 *framePointer = &panel->mainSprite.paletteFrame;
            {
                register u32 oldFrame asm("r3") = *framePointer;
                asm("" : : "r"(oldFrame));
                if (frame != oldFrame) {
                    *framePointer = frame;
                    {
                        struct ScenePanelGraphics *graphics = &panel->mainSprite.graphics;
                        u32 bank;
                        u32 frameValue;
                        bank = panel->targetPalette * 4;
                        frameValue = *framePointer;
                        {
                            register u32 paletteBase asm("r5") = 0x203;
                            asm("" : "+r"(paletteBase));
                            frameValue += paletteBase;
                        }
                        bank += frameValue;
                        AnimData_SelectPaletteBank(graphics, bank);
                    }
                }
            }
        }
    }
}
