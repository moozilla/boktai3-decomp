#include "global.h"
#include "scene_actor_reset.h"

s32 Math_Atan2(s32, s32);
struct ActorReactionTarget {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
struct ActorReactionSensor {
    struct ActorReactionTarget *target;
    u8 unknown4[0xB];
    u8 flags;
    u32 unknown10;
    u32 distance, range;
};
struct ActorReactionDelta {
    s16 x, y, z;
};
struct ActorReactionContext {
    u8 unknown0[0x8E8];
    u8 request, direction;
};
static inline u8 HasReactionFlag(struct SceneActorRegistryResetView *s)
{
    struct ActorReactionSensor *sensor = (void *)&s->tracking.target;
    u32 mask = 2;
    if (sensor->flags & mask)
        return 1;
    return 0;
}
s32 SceneActor_TryRequestFacingReaction(struct SceneActorRegistryResetView *s)
{
    u32 range;
    struct ActorReactionDelta delta;
    struct ActorReactionSensor *sensor, *sensor2;
    struct ActorReactionTarget *target;
    s32 angle, height;
    struct ActorReactionDelta *out;
    struct ActorReactionContext *context;
    if (!HasReactionFlag(s))
        return 0;
    sensor = (void *)&s->tracking.target;
    {
        u32 mask = -3;
        sensor->flags &= mask;
    }
    {
        u32 kind = s->field2B5;
        sensor2 = sensor;
        if (kind == 0x27)
            range = s->tracking.range * 2;
        else
            range = s->tracking.range;
    }
    if (s->tracking.distance > range)
        return 0;
    {
        const s16 *own;
        const s16 *other;
        struct ActorReactionTarget *t;
        out = &delta;
        own = (const s16 *)&s->body.position;
        t = sensor2->target;
        other = (const s16 *)&t->position;
        delta.x = *(s16 *)((u8 *)t + 0xc) - own[0];
        out->y = other[1] - own[1];
        out->z = other[2] - own[2];
    }
    angle = ((Math_Atan2(delta.x, out->z) + 0x10) & 0xFF) >> 6;
    if (angle != (((s->body.direction + 0x10) & 0xFF) >> 6))
        return 0;
    height = out->y;
    if (height < 0)
        height = -height;
    if (height > 0x1FF)
        return 0;
    context = s->context;
    context->direction = ((s->tracking.heading + 0x80) & 0xFF) >> 6;
    context->request = 0xE;
    return 1;
}
