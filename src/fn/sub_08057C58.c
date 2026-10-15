#include "scene_panel_effects.h"
#define ScenePanel_IsSpriteVisible sub_08057C58
s32 ScenePanel_IsSpriteVisible(struct ScenePanelMainSprite *sprite)
{
    if (sprite->active && !(sprite->flags & 1))
        return 1;
    return 0;
}
