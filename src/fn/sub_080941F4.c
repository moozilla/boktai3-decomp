#include "global.h"
#include "scene_actor_control_phase.h"
#include "engine.h"

extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void sub_0809345C(void *, void *);
void *Bee_SubVector(void *, void *, void *);

struct BeeRouteAnchor {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
struct BeeAnchorRoute {
    u8 unknown0[0x3E0];
    struct ContactPosition points[9];
    u8 index;
    u8 unknown429[0x3];
    u16 speed;
    u8 unknown42E[0xC];
    u16 timer;
    u16 unknown43C;
    u16 polarSpeed;
    struct BeeRouteAnchor *anchor;
    u8 unknown444[0x3C];
    struct ContactPosition motion;
};

static inline s32 Bee_ScalePolarComponent(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline s32 Bee_ScaleDuration(s32 n)
{
    if (n >= 0)
        return n >> 5;
    return -((-n) >> 5);
}
static inline void Bee_MakePolarVector(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Bee_ScalePolarComponent(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Bee_ScalePolarComponent(gUnk_086149C4[angle & 255] * speed);
}
static inline void Bee_SubtractPositions(struct ContactPosition *out,
                                         const struct ContactPosition *from,
                                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u16 Bee_NextRandom(void)
{
    u32 offset;
    const u16 *table;

    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    return *(const u16 *)(offset + (u32)table);
}
static inline u32 Bee_OffsetHeading(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}
static inline void Bee_InstallControl(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    u8 *taken = &a->field2B2;
    zero = 0;
    *taken = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
static inline void Bee_AddPosition(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = out->x + from->x;
    out->y = out->y + from->y;
    out->z = out->z + from->z;
}
static inline s32 Bee_Max(s32 a, s32 b)
{
    if (a < b)
        return b;
    return a;
}
static inline u16 Bee_GetPreviousX(struct BeeAnchorRoute *route, s32 index)
{
    struct BeeAnchorRoute *point = (struct BeeAnchorRoute *)((u32)route + index * 8);
    return point->points[0].x;
}
static inline u16 Bee_GetPreviousZ(struct BeeAnchorRoute *route, s32 index)
{
    struct BeeAnchorRoute *point = (struct BeeAnchorRoute *)((u32)route + index * 8);
    return point->points[0].z;
}
// confidence=low: steps along nine route points toward an anchor, then retargets after the route ends.
void sub_080941F4(struct SceneActorRegistryResetView *actor)
{
    struct BeeAnchorRoute *route = actor->context;
    struct ContactPosition delta, target, step;
    s32 n;

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        Bee_InstallControl(actor, actor->entryTarget228, 1);
        route->index = 0;
        Bee_SubtractPositions(&delta, &actor->body.position, &route->anchor->position);
        actor->tracking.heading = Math_Atan2(delta.x, delta.z);
        Bee_MakePolarVector(&target, Bee_OffsetHeading(Bee_NextRandom(), &actor->tracking.heading) & 255,
                            route->polarSpeed);
        route->timer = 0x500;
        Bee_AddPosition(&target, &actor->body.position);
        sub_0809345C(actor, &target);
        Bee_SubVector(actor, &actor->body.position, &target);
        ZeroFill8(&route->motion);
    }
    Bee_SubtractPositions(&step, &actor->body.position, &route->points[route->index]);
    Bee_SubtractPositions(&delta, &actor->body.position, &route->anchor->position);
    {
        u32 heading = Math_Atan2(delta.x, delta.z);
        actor->tracking.heading = heading;
        actor->body.direction = heading;
    }
    if ((s16)route->timer > 0 && step.x * step.x + step.z * step.z > 1024) {
        n = Bee_Max(Bee_ScaleDuration((s16)route->timer), 1);
        actor->body.motion.x += Div(route->points[route->index].x - actor->body.position.x, n);
        actor->body.motion.z += Div(route->points[route->index].z - actor->body.position.z, n);
        route->motion = actor->body.motion;
        route->timer -= actor->counter2AC;
    } else {
        route->index++;
        if (route->index > 8) {
            actor->body.position.x = route->points[8].x;
            actor->body.position.z = route->points[8].z;
            Bee_SubtractPositions(&delta, &actor->body.position, &route->anchor->position);
            actor->tracking.heading = Math_Atan2(delta.x, delta.z);
            Bee_MakePolarVector(&target, Bee_OffsetHeading(Bee_NextRandom(), &actor->tracking.heading) & 255,
                                route->polarSpeed);
            Bee_AddPosition(&target, &actor->body.position);
            sub_0809345C(actor, &target);
            Bee_SubVector(actor, &actor->body.position, &target);
            {
                u32 duration = route->speed << 5;
                route->timer = duration;
            }
            route->index = 0;
            actor->counter2AC = 0;
            return;
        }
        actor->body.position.x = Bee_GetPreviousX(route, route->index - 1);
        actor->body.position.z = Bee_GetPreviousZ(route, route->index - 1);
    }
    if ((s32)actor->counter2AC <= 19)
        actor->counter2AC++;
}
