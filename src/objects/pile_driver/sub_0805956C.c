#include "objects/pile_driver/pile_driver_rig.h"
void PileDriverRig_ShowSprites(struct PileDriverRig *rig)
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
