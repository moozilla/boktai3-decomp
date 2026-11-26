#include "objects/pile_driver/scene_panel_effects.h"

// Blends the panel palette (16 rows) and the sprite palette from the previous time-of-day
// palette toward the target one over 64 frames. Each channel is read through the u16 pointer
// (README fact 37), which keeps the 31 masks spilled as in the ROM. In the sprite loop the target
// and destination are indexed, so loop.c creates their pointers after the hoisted masks and both
// outrank them.
void ScenePanel_UpdateTimePalette(struct ScenePanelEffectsView *panel)
{
    u32 phase = SunClock_GetPhase();
    if (panel->timePhase != phase) {
        panel->timePhase = phase;
        ScenePanel_SelectTimePalette((struct ScenePanel *)panel, panel->timePhase);
    }
    if (panel->paletteBlendActive) {
        panel->channelsDirty = 1;
        if (++panel->paletteBlendFrame > 63) {
            u16 *destination;
            const u16 *source;
            u32 offset = panel->targetPalette * 4;
            const u16 **sources = panel->paletteSources;
            s32 i;
            sources = (const u16 **)((u8 *)sources + offset);
            destination = panel->paletteColors[0];
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
                    sprite->paletteFrame = ScenePanel_AdvancePaletteFrame(sprite);
                    {
                        struct ScenePanelGraphics *graphics = &sprite->graphics;
                        u32 bank;
                        u32 paletteFrame;
                        bank = panel->targetPalette * 4;
                        paletteFrame = sprite->paletteFrame;
                        paletteFrame += 0x203;
                        bank += paletteFrame;
                        AnimData_SelectPaletteBank(graphics, bank);
                    }
                }
            }
        } else {
            s32 fromWeight = 64 - panel->paletteBlendFrame;
            s32 toWeight = panel->paletteBlendFrame;
            const u16 *from = panel->paletteSources[panel->previousPalette];
            const u16 *to = panel->paletteSources[panel->targetPalette];
            s32 row, column;
            for (row = 0; row <= 15; row++) {
                for (column = 0; column <= 15; column++) {
                    s32 redFrom = *from & 31;
                    s32 greenFrom = (*from >> 5) & 31;
                    s32 blueFrom = (*from >> 10) & 31;
                    s32 redTo = *to & 31;
                    s32 greenTo = (*to >> 5) & 31;
                    s32 blueTo = (*to >> 10) & 31;
                    s32 red, green, blue;
                    red = (fromWeight * redFrom + toWeight * redTo) >> 6;
                    green = (fromWeight * greenFrom + toWeight * greenTo) >> 6;
                    blue = (fromWeight * blueFrom + toWeight * blueTo) >> 6;
                    panel->paletteColors[row][column] = (blue << 10) | (green << 5) | red;
                    from++;
                    to++;
                }
            }
            if (ScenePanel_IsSpriteVisible(&panel->mainSprite)) {
                s32 frame = ScenePanel_AdvancePaletteFrame(&panel->mainSprite);
                u32 offset;
                const u16 *previous, *target;
                u8 **banks = &gPaletteBankData;
                u8 *bankBase;
                u16 *destination;
                s32 i;
                offset = panel->previousPalette * 4;
                frame += 0x203;
                offset += frame;
                offset <<= 5;
                bankBase = *banks;
                previous = (const u16 *)(bankBase + offset);
                offset = panel->targetPalette * 4;
                offset += frame;
                offset <<= 5;
                target = (const u16 *)(bankBase + offset);
                destination = panel->mainSprite.paletteColors;
                for (i = 0; i <= 15; i++) {
                    u16 *out = &destination[i];
                    s32 redFrom = *previous & 31;
                    s32 greenFrom = (*previous >> 5) & 31;
                    s32 blueFrom = (*previous >> 10) & 31;
                    s32 redTo = target[i] & 31;
                    s32 greenTo = (target[i] >> 5) & 31;
                    s32 blueTo = (target[i] >> 10) & 31;
                    s32 red, green, blue;
                    red = (fromWeight * redFrom + toWeight * redTo) >> 6;
                    green = (fromWeight * greenFrom + toWeight * greenTo) >> 6;
                    blue = (fromWeight * blueFrom + toWeight * blueTo) >> 6;
                    *out = (blue << 10) | (green << 5) | red;
                    previous++;
                }
                panel->mainSprite.graphics.paletteOverride = panel->mainSprite.paletteColors;
            }
        }
    } else {
        struct ScenePanelMainSprite *sprite = &panel->mainSprite;
        if (ScenePanel_IsSpriteVisible(sprite)) {
            s32 frame = ScenePanel_AdvancePaletteFrame(sprite);
            if (frame != sprite->paletteFrame) {
                sprite->paletteFrame = frame;
                {
                    struct ScenePanelGraphics *graphics = &sprite->graphics;
                    u32 bank;
                    u32 paletteFrame;
                    bank = panel->targetPalette * 4;
                    paletteFrame = sprite->paletteFrame;
                    paletteFrame += 0x203;
                    bank += paletteFrame;
                    AnimData_SelectPaletteBank(graphics, bank);
                }
            }
        }
    }
}
