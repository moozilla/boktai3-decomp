#include "scene_panel_effects.h"
#define ScenePanel_UpdatePaletteState sub_080580C8
void ScenePanel_UpdatePaletteState(struct ScenePanel *input)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    if (panel->state)
        panel->state(input);
    sub_08057E04(input);
    if (panel->channelsDirty) {
        s32 i;
        for (i = 0; i < 3; i++)
            CpuSet(panel->paletteColors[panel->channels[i]], gUnk_03004FC0 + i * 16, 0x04000008);
        panel->channelsDirty = 0;
    }
}
