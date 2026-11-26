#include "global.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/actor/scene_actor_reset.h"

s32 Math_Atan2(s32, s32);
struct ActorReactionDelta {
    s16 x, y, z;
};
struct ActorReactionContext {
    u8 unknown0[0x8E8];
    u8 request, direction;
};
s32 SceneActor_TryRequestFacingReaction(struct ChaseActor *s)
{
    u32 range;
    struct ActorReactionDelta delta;
    struct SceneActorChaseTracking *sensor, *sensor2;
    struct SceneActorChaseTarget *target;
    s32 angle, height;
    struct ActorReactionDelta *out;
    struct ActorReactionContext *context;
    if (!SceneActorChase_TrackingFlags(&s->base.tracking, 2))
        return 0;
    sensor = &s->base.tracking;
    {
        u32 mask = -3;
        sensor->flags &= mask;
    }
    {
        u32 kind = s->base.controlKind;
        sensor2 = sensor;
        if (kind == 0x27)
            range = s->base.tracking.range * 2;
        else
            range = s->base.tracking.range;
    }
    if (s->base.tracking.distance > range)
        return 0;
    {
        const s16 *own;
        const s16 *other;
        struct SceneActorChaseTarget *t;
        out = &delta;
        own = (const s16 *)&s->base.body.position;
        t = sensor2->target;
        other = (const s16 *)&t->position;
        delta.x = t->position.x - own[0];
        out->y = other[1] - own[1];
        out->z = other[2] - own[2];
    }
    angle = ((Math_Atan2(delta.x, out->z) + 0x10) & 0xFF) >> 6;
    if (angle != (((s->base.body.direction + 0x10) & 0xFF) >> 6))
        return 0;
    height = out->y;
    if (height < 0)
        height = -height;
    if (height > 0x1FF)
        return 0;
    context = s->base.context;
    context->direction = ((s->base.tracking.heading + 0x80) & 0xFF) >> 6;
    context->request = 0xE;
    return 1;
}
