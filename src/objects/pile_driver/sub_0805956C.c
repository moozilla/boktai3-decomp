#include "objects/pile_driver/scene_panel_rig.h"
void ScenePanelRig_ShowSprites(struct ScenePanelRig *rig)
{
    s32 i;
    rig->main.flags &= ~1;
    {
        u32 mask = ~1;
        struct SceneRigSprite *sprite = rig->primary;
        i = 7;
        do {
            sprite->flags &= mask;
            sprite++;
        } while (--i >= 0);
    }
    {
        u32 mask = ~1;
        struct SceneRigSprite *sprite = rig->secondary;
        i = 5;
        do {
            sprite->flags &= mask;
            sprite++;
        } while (--i >= 0);
    }
}
