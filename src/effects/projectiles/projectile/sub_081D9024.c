#include "effects/projectiles/projectile_task.h"
void TimedEffect_UpdateHoming(void);
s32 TimedEffect_BeginPhase26(struct Task4E01Projectile *effect)
{
    struct Task4E01Params *config = &effect->params;
    struct ContactSwarmSprite *sprite;
    Projectile_SetUpdateCallback(config, TimedEffect_UpdateHoming);
    sprite = &effect->sprite;
    {
        u32 phaseId = 0x1A;
        sprite->frame = phaseId;
        effect->target = NULL;
    }
    if (config->lifetime < 120)
        config->lifetime = 120;
    return 0;
}
