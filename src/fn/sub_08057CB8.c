// MATCHING_CONCESSION: register-pinning
#include "scene_panel_effects.h"
#define ScenePanel_InitTimePalette sub_08057CB8
void ScenePanel_InitTimePalette(struct ScenePanel *input)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    u32 resourceIds[3];
    s32 i;
    const u16 *colors;
    const u16 *source;
    u16 *destination;
    if (Script_SeekToKeyword('A')) {
        u32 *write = resourceIds;
        i = 2;
        do {
            *write++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        resourceIds[0] = 0x2365;
        resourceIds[1] = 0xF80C;
        resourceIds[2] = 0xA344;
    }
    {
        u32 *read;
        i = 0;
        read = resourceIds;
        do {
            u32 offset = i << 2;
            u8 *resource;
            resource = sub_0821A520(0x92B3, (u16)*read++);
            {
                u8 *base = (u8 *)panel + 0x220;
                base += offset;
                *(const u16 **)base = (u16 *)(resource + 0x14);
            }
            i++;
        } while (i < 3);
    }
    panel->timePhase = sub_08228DB8();
    sub_08057B5C((struct ScenePanel *)panel, panel->timePhase);
    panel->paletteBlendActive = 0;
    panel->paletteBlendFrame = 0;
    colors = panel->paletteSources[panel->targetPalette];
    {
        register u8 *base asm("r6") = (u8 *)panel;
        asm("" : "+r"(base));
        destination = (u16 *)(base + 0x20);
    }
    source = colors;
    i = 15;
    do {
        CpuSet(source, destination, 0x04000008);
        destination += 16;
        source += 16;
    } while (--i >= 0);
    destination = gUnk_03004FC0;
    source = colors;
    i = 2;
    do {
        CpuSet(source, destination, 0x04000008);
        destination += 16;
        source += 16;
    } while (--i >= 0);
    i = 2;
    {
        u16 *channel = &panel->channels[2];
        do {
            *channel-- = i;
        } while (--i >= 0);
    }
    {
        struct ScenePanelMainSprite *sprite = &panel->mainSprite;
        if (sub_08057C58(sprite)) {
            sprite->paletteFrame = sub_08057C78(sprite);
            {
                struct ScenePanelGraphics *graphics = &sprite->graphics;
                u32 bank;
                u32 paletteFrame;
                bank = panel->targetPalette * 4;
                paletteFrame = sprite->paletteFrame;
                paletteFrame += 0x203;
                bank += paletteFrame;
                sub_08215284(graphics, bank);
            }
        }
    }
    panel->state = 0;
}
