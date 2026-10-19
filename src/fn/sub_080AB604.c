
#ifdef NONMATCHING
// fuzzy: 92.4%
// foundry draft (worker best 91.5%); what differs is mostly register allocation and instruction order
#define SceneActor_TryRequestFacingReaction sub_080AB604
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
struct ActorReactionContext {
    u8 unknown0[0x8E8];
    u8 request, direction;
};
static inline u8 HasReactionFlag(struct SceneActorRegistryResetView *s)
{
    struct ActorReactionSensor *sensor = (void *)&s->fieldDC;
    u32 mask = 2;
    if (sensor->flags & mask)
        return 1;
    return 0;
}
s32 sub_080AB604(struct SceneActorRegistryResetView *s)
{
    u32 range;
    struct {
        s16 v[3];
    } delta;
    struct ActorReactionSensor *sensor;
    struct ActorReactionTarget *target;
    s32 angle, height;
    s16 *out;
    struct ActorReactionContext *context;
    if (!HasReactionFlag(s))
        return 0;
    sensor = (void *)&s->fieldDC;
    {
        u32 mask = -3;
        sensor->flags &= mask;
    }
    if (s->field2B5 == 0x27)
        range = s->rangeF4 * 2;
    else
        range = s->rangeF4;
    if (s->distanceF0 > range)
        return 0;
    {
        const s16 *own;
        const s16 *other;
        out = delta.v;
        own = (const s16 *)&s->position;
        other = (const s16 *)&sensor->target->position;
        out[0] = other[0] - own[0];
        out[1] = other[1] - own[1];
        out[2] = other[2] - own[2];
    }
    angle = ((Math_Atan2(delta.v[0], out[2]) + 0x10) & 0xFF) >> 6;
    if (angle != (((s->direction + 0x10) & 0xFF) >> 6))
        return 0;
    height = out[1];
    if (height < 0)
        height = -height;
    if (height > 0x1FF)
        return 0;
    context = s->context;
    context->direction = ((s->fieldE9 + 0x80) & 0xFF) >> 6;
    context->request = 0xE;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080AB604);
#endif
