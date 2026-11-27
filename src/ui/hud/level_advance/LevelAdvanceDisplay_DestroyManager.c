#include "ui/hud/level_advance_display.h"
s32 LevelAdvanceDisplay_DestroyManager(struct LevelAdvanceManager *manager)
{
    s32 i;
    Sprite_Remove(&manager->sprite);
    for (i = 0; i < 8; i++)
        Effect_Remove(&manager->particles[i].sprite);
    gLevelAdvanceManager = 0;
    return 0;
}
