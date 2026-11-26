#include "objects/pile_driver/pile_driver_effects.h"
s32 PileDriver_IsSpriteVisible(struct ScenePanelMainSprite *sprite)
{
    if (sprite->active && !(sprite->flags & 1))
        return 1;
    return 0;
}
