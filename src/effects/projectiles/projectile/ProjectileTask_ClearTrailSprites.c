#include "effects/projectiles/timed_effect_manager.h"

void ProjectileTask_ClearTrailSprites(struct TimedEffectManager *manager)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        manager->effects[i].active = 0;
        manager->effects[i].sprite.flags |= 1;
    }
}
