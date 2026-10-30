#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void sub_0809345C(void *, void *);
void Bee_SubVector(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
struct BeeRoute {
    u8 unknown0[0x3D4];
    u32 flags;
    u8 unknown3D8[4];
    u8 returnMode;
    u8 unknown3DD[3];
    struct ContactPosition points[9];
    u8 index, unknown429, cooldown, unknown42B;
    u16 duration;
    u8 unknown42E[12];
    s16 remaining;
    u8 unknown43C[0x480 - 0x43C];
    struct ContactPosition motion;
};
static inline u8 Bee_TakeEntryFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Bee_TestRouteFlags(struct BeeRoute *body, u32 mask)
{
    u32 flags = body->flags;
    if (flags & mask)
        return 1;
    return 0;
}
static inline void Bee_ClearRouteFlags(struct BeeRoute *body, u32 bits)
{
    u32 mask = ~bits;
    body->flags &= mask;
}
static inline u32 Bee_OffsetHeading(u32 value, const u8 *heading)
{
    return (value & 63) + *heading;
}
static inline u16 Bee_GetWaypointX(struct BeeRoute *body, s32 index)
{
    struct BeeRoute *point = (struct BeeRoute *)(index * 8 + (u32)body);
    return point->points[0].x;
}
static inline u16 Bee_GetWaypointZ(struct BeeRoute *body, s32 index)
{
    struct BeeRoute *point = (struct BeeRoute *)(index * 8 + (u32)body);
    return point->points[0].z;
}
static inline s32 Bee_Max(s32 a, s32 b)
{
    if (a < b)
        return b;
    return a;
}
static inline s32 Bee_TruncateShift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u16 Bee_NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR);
}
static inline void Bee_MakePolarVector(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Bee_TruncateShift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Bee_TruncateShift(gUnk_086149C4[angle & 255] * speed, 12);
}
static inline void Bee_SubtractPositions(struct ContactPosition *out,
                                         const struct ContactPosition *from,
                                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void Bee_AddPosition(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = out->x + from->x;
    out->y = out->y + from->y;
    out->z = out->z + from->z;
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
static inline void Bee_ClearVector(vu32 *zero, void *out)
{
    *zero = 0;
    CpuSet((const void *)zero, out, 0x05000002);
}
static inline void Bee_EnterState(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

static inline u8 Bee_GetHeading(const struct SceneActorRegistryResetView *actor)
{
    return actor->tracking.heading;
}

// confidence=low: follows a randomized route and returns to the Bee behavior selector.
void Bee_FollowWanderRoute(struct SceneActorRegistryResetView *actor)
{
    vu32 zero;
    struct ContactPosition dest;
    struct ContactPosition delta;
    struct BeeRoute *body = actor->context;
    if (Bee_TakeEntryFlag(&actor->field2B1)) {
        Bee_InstallControl(actor, actor->entryTarget228, 1);
        body->index = 0;
        if (Bee_TestRouteFlags(body, 8)) {
            {
                u8 reverseBias = 128;
                Bee_MakePolarVector(
                    &dest, (actor->tracking.heading + ((Bee_NextRandom() & 63) + reverseBias)) & 255, 320);
            }
            Bee_ClearRouteFlags(body, 8);
            {
                s32 variant = Bee_GetMovementVariant(actor);
                s32 duration = 6 << variant;
                body->remaining = duration;
            }
        } else {
            Bee_MakePolarVector(&dest, Bee_OffsetHeading(Bee_NextRandom(), &actor->tracking.heading) & 255,
                                512);
            {
                s32 variant = Bee_GetMovementVariant(actor);
                s32 duration = 40 << variant;
                body->remaining = duration;
            }
        }
        Bee_AddPosition(&dest, &actor->body.position);
        sub_0809345C(actor, &dest);
        Bee_SubVector(actor, &actor->body.position, &dest);
        Bee_ClearVector(&zero, &body->motion);
    }
    Bee_SubtractPositions(&delta, &actor->body.position, &body->points[body->index]);
    {
        u8 facing = Bee_GetHeading(actor);
        actor->body.direction = facing;
    }
    if ((body->remaining > 0) && (((delta.x * delta.x) + (delta.z * delta.z)) > 1024)) {
        s32 steps = Bee_Max(Bee_TruncateShift(body->remaining, 5), 1);
        actor->body.motion.x += Div(body->points[body->index].x - actor->body.position.x, steps);
        actor->body.motion.z += Div(body->points[body->index].z - actor->body.position.z, steps);
        body->motion = actor->body.motion;
        body->remaining -= actor->counter2AC;
    } else if ((++body->index) > 8) {
        actor->body.position.x = body->points[8].x;
        actor->body.position.z = body->points[8].z;
        Bee_MakePolarVector(&dest, ((Bee_NextRandom() & 63) + actor->tracking.heading) & 255, 512);
        Bee_AddPosition(&dest, &actor->body.position);
        sub_0809345C(actor, &dest);
        Bee_SubVector(actor, &actor->body.position, &dest);
        {
            s32 variant = Bee_GetMovementVariant(actor);
            s32 duration = (body->duration) << variant;
            body->remaining = duration;
        }
        body->index = 0;
        actor->counter2AC = 0;
        return;
    } else {
        actor->body.position.x = Bee_GetWaypointX(body, body->index - 1);
        actor->body.position.z = Bee_GetWaypointZ(body, body->index - 1);
    }
    if (((s32)actor->counter2AC) <= 19)
        actor->counter2AC++;
    if (body->cooldown)
        body->cooldown--;
    else if (actor->tracking.distance <= 0xFFFE) {
        if (body->returnMode)
            Bee_EnterState(actor, 2, 1);
        else
            Bee_EnterState(actor, 2, 2);
    }
}
