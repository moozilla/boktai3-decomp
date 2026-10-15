#include "scene_panel_effects.h"
#define ScenePanel_AdvancePaletteFrame sub_08057C78
s32 ScenePanel_AdvancePaletteFrame(struct ScenePanelMainSprite *sprite)
{
    u16 *counter = &sprite->phaseCounter;
    u32 frame;
    (*counter)++;
    if (*counter > 129)
        *counter = 0;
    frame = *counter;
    if (frame <= 29)
        return 0;
    if (frame <= 39)
        return 1;
    if (frame <= 49)
        return 2;
    if (frame <= 109)
        return 3;
    if (frame <= 119)
        return 2;
    return 1;
}
