#include "engine/shared/palette_resource.h"
#include "menus/scene_panel_presentation/scene_panel_effects.h"
#include "gba/syscall.h"
void ScenePanel_InitTimePalette(struct ScenePanel *input)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    u32 resourceIds[3];
    s32 i;
    const u16 *colors;
    const u16 *source;
    u16 *destination;
    if (Script_SeekToKeyword('A')) {
        for (i = 0; i < 3; i++)
            resourceIds[i] = Script_GetValue();
    } else {
        resourceIds[0] = 0x2365;
        resourceIds[1] = 0xF80C;
        resourceIds[2] = 0xA344;
    }
    for (i = 0; i < 3; i++) {
        struct PaletteResource *resource = ScriptProc_Start(0x92B3, (u16)resourceIds[i]);
        panel->paletteSources[i] = resource->banks[0];
    }
    panel->timePhase = SunClock_GetPhase();
    ScenePanel_SelectTimePalette((struct ScenePanel *)panel, panel->timePhase);
    panel->paletteBlendActive = 0;
    panel->paletteBlendFrame = 0;
    colors = panel->paletteSources[panel->targetPalette];
    for (i = 0; i < 16; i++)
        CpuCopy32(&colors[i * 16], panel->paletteColors[i], 32);
    destination = gBgPaletteBuffer;
    source = colors;
    i = 2;
    do {
        CpuCopy32(source, destination, 32);
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
    panel->state = 0;
}
