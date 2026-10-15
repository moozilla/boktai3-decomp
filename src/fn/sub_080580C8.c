#include "scene_panel_effects.h"
void ScenePanel_UpdatePaletteState(struct ScenePanel *input)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    if (panel->state)
        panel->state(input);
    ScenePanel_UpdateTimePalette(input);
    if (panel->channelsDirty) {
        s32 i;
        for (i = 0; i < 3; i++)
            CpuSet(panel->paletteColors[panel->channels[i]], gUnk_03004FC0 + i * 16, 0x04000008);
        panel->channelsDirty = 0;
    }
}
