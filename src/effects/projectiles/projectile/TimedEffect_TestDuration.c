#include "effects/projectiles/projectile_task.h"
s32 TimedEffect_TestDuration(struct Task4E01Projectile *effect)
{
    struct Task4E01Params *config = &effect->params;
    if ((effect->frame & 1) + 1 == ((config->flags & 0xF00) >> 8)) {
        u32 mask = 2;
        effect->flags |= mask;
    }
    if (effect->frame >= config->lifetime)
        return 2;
    return 0;
}
