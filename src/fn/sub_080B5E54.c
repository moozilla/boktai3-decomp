#include "global.h"
#include "scene_actor_timed_behavior.h"
static inline u8 HasTargetFlag(struct SceneActorReactionMotion *s)
{
    u32 mask = 0x10;
    if (s->flags & mask)
        return 1;
    return 0;
}
u32 SceneActorControl_CanTurnTowardTarget(struct SceneActorRegistryResetView *s)
{
    struct SceneActorTimedBehaviorContext *context = s->context;
    s32 x, y, z, angle;
    if (!s->tracking.target)
        return 0;
    if (context->timer704)
        return 0;
    if (!HasTargetFlag((void *)&s->tracking.target))
        return 0;
    {
        struct ContactPosition *v = &s->secondaryMotion;
        s32 sum;
        x = v->x;
        sum = x * x;
        x = v->y;
        sum += x * x;
        x = v->z;
        if (sum + x * x != 0)
            return 0;
    }
    if (s->tracking.distance > 0x24000)
        return 0;
    angle = ((struct SceneActorFacingTarget *)s->tracking.target)->heading - s->body.direction;
    if (angle > 128)
        angle -= 256;
    else if (angle < -128)
        angle += 256;
    x = angle;
    if (x < 0)
        x = -x;
    if (x > 63 && !context->field6D0)
        return 1;
    return 0;
}
