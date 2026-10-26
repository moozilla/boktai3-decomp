#include "level_advance_display.h"
s32 LevelAdvanceDisplay_DestroyManager(struct LevelAdvanceManager *manager)
{
    struct LevelAdvanceParticle *particle;
    s32 i;
    Sprite_Remove(&manager->sprite);
    particle = manager->particles;
    for (i = 7; i >= 0; i--) {
        Effect_Remove(particle);
        particle++;
    }
    {
        u32 zero = 0;
        gLevelAdvanceManager = (void *)zero;
    }
    return 0;
}
