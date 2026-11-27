#include "effects/projectiles/projectile_task.h"
void Boss_FinalExplosions(void);
s32 TimedEffect_UpdateDeceleration(struct Task4E01Projectile *effect)
{
    struct Task4E01Params *config = &effect->params;
    /* Arc flight reuses the lifetime word as vertical acceleration. */
    effect->runtime.elapsed -= effect->lifetime;
    effect->lifetime--;
    if (effect->frame >= config->lifetime || effect->sprite.position.y < 0 ||
        effect->sprite.position.y < effect->runtime.arcStartHeight)
        Projectile_SetUpdateCallback(config, Boss_FinalExplosions);
    return 0;
}
