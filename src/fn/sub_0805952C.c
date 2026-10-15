#include "scene_panel_rig.h"
void ScenePanelRig_HideSprites(struct ScenePanelRig *rig)
{
    struct SceneRigSprite *sprite;
    s32 i;
    u32 mask;
    rig->main.flags |= 1;
    mask = 1;
    sprite = rig->primary;
    i = 7;
    do {
        sprite->flags |= mask;
        sprite++;
    } while (--i >= 0);
    mask = 1;
    sprite = rig->secondary;
    i = 5;
    do {
        sprite->flags |= mask;
        sprite++;
    } while (--i >= 0);
}
