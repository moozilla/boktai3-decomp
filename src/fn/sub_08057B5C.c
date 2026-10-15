#include "scene_panel_effects.h"
#define ScenePanel_SelectTimePalette sub_08057B5C
void ScenePanel_SelectTimePalette(struct ScenePanel *input, s32 phase)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    switch (panel->timePhase) {
        case 0:
            panel->previousPalette = 2;
            panel->targetPalette = 2;
            break;
        case 1:
            panel->previousPalette = 2;
            panel->targetPalette = 0;
            panel->paletteBlendActive = 1;
            panel->paletteBlendFrame = 0;
            break;
        case 2:
            panel->previousPalette = 0;
            panel->targetPalette = 0;
            break;
        case 3:
            panel->previousPalette = 0;
            panel->targetPalette = 1;
            panel->paletteBlendActive = 1;
            panel->paletteBlendFrame = 0;
            break;
        case 4:
            panel->previousPalette = 1;
            panel->targetPalette = 2;
            panel->paletteBlendActive = 1;
            panel->paletteBlendFrame = 0;
            break;
        case 5:
            panel->previousPalette = 2;
            panel->targetPalette = 2;
            break;
    }
}
