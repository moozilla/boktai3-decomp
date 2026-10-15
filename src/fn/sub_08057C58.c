#include "scene_panel_effects.h"
s32 ScenePanel_IsSpriteVisible(struct ScenePanelMainSprite *sprite)
{
    if (sprite->active && !(sprite->flags & 1))
        return 1;
    return 0;
}
