#include "scene_panel_rig_setup.h"
void ScenePanelRig_RemoveSprites(struct ScenePanelRig *rig)
{
    struct SceneRigSprite *sprite;
    s32 remaining;
    Sprite_Remove(&rig->main);
    sprite = rig->primary;
    remaining = 7;
    do {
        Sprite_Remove(sprite);
        sprite++;
        remaining--;
    } while (remaining >= 0);
    sprite = rig->secondary;
    remaining = 5;
    do {
        Sprite_Remove(sprite);
        sprite++;
        remaining--;
    } while (remaining >= 0);
}
