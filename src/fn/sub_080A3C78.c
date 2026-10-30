#include "global.h"
#include "proximity_emitter.h"
#include "scene_actor_reset.h"

s32 SceneActor_IsWithinTetherRange(struct SceneActorRegistryResetView *actor, s32 xRange,
                                   s32 zRange)
{
    struct ContactPosition *target = TetherActor_GetPosition();
    if (!target)
        return 0;
    {
        s16 delta[4];
        s16 *p = delta;
        struct ContactPosition *pos = &actor->body.position;
        {
            s16 value = target->x - pos->x;
            s16 *q = delta;
            *q = value;
        }
        p[1] = target->y - pos->y;
        p[2] = target->z - pos->z;
        {
            s32 dx = delta[0];
            if (dx < 0)
                dx = -dx;
            if (dx > xRange)
                return 0;
        }
        {
            s32 dz = p[2];
            if (dz < 0)
                dz = -dz;
            if (dz > zRange)
                return 0;
        }
        return 1;
    }
}
