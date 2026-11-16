#include "global.h"
#include "skeleton.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "player.h"

struct ApproachRoutePoint {
    s16 x, z;
};
struct ApproachRouteLink {
    u8 unknown0[8];
    struct ApproachRoutePoint *point;
};
struct ApproachSaved {
    void *target;
    u8 state, phase, behavior, kind;
};
struct ApproachGroup {
    u8 unknown0[0x40];
    u16 flags414, field416, field418;
    u8 unknown46[2];
    void *entryControl41C;
    u8 unknown4C[0x28];
    struct ApproachRouteLink link;
    u8 unknown80[0xC];
    struct ApproachSaved saved;
    u8 unknown94[0x168];
    void *routeObject;
    struct ContactPosition *destination;
    u8 direction, savedDirection, timer, unknown207;
    u8 unknown208[0x14];
    void *control;
    u8 unknown220[4];
    void *failure;
};

static inline void SetControl(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void SetState(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline u8 Chase(struct ChaseActor *actor, struct ApproachGroup *group, u32 bits)
{
    void *route = group->routeObject;
    struct ContactPosition *position = &actor->base.body.position;
    u8 *target = *(u8 **)&actor->base.tracking.target;
    struct ContactPosition *destination = (void *)(target + 12);
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            actor->base.flags2D4 &= ~bits;
        }
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->base.body.motion, 8);
            return 1;
        } else {
            {
                actor->base.flags2D4 &= ~bits;
            }
            return 0;
        }
    }
}

static inline u16 MotionFlags(struct ChaseActor *actor, u32 mask)
{
    u16 value = actor->base.flags2D4;
    value &= mask;
    return value;
}
static inline void OrFlags(u16 *flags, u32 mask)
{
    u32 value = mask;
    value |= *flags;
    *flags = value;
}

static inline u32 Flag(struct ChaseActor *a, u32 m, u32 off)
{
    return off == 0x2D4 ? *(u16 *)((u8 *)a + off) & m : *(u32 *)((u8 *)a + off) & m;
}
static inline u8 Unavailable(struct ChaseActor *a)
{
    if (a && (Flag(a, 0x40, 0x2D4) || Flag(a, 0x20000, 0x2D0) || a->base.health <= 0))
        return 1;
    return 0;
}
s32 Div(s32, s32);
static inline u8 Pending(u8 *context)
{
    u32 mask = 0x10;
    if (*(u32 *)(context + 0x6DC) & mask)
        return 1;
    return 0;
}
static inline s32 MotionSquared(struct ContactPosition *p)
{
    return p->x * p->x + p->y * p->y + p->z * p->z;
}
// confidence=low: starts pursuit when stationary, with a six-second cooldown.
void Skeleton_BeginStationaryPursuit(struct ChaseActor *a)
{
    u8 *context;
    struct ApproachGroup *group;
    if (Unavailable(a))
        return;
    context = a->base.context;
    group = (void *)&a->group.common;
    if (Pending(context))
        return;
    if (Flag(a, 0xA03F, 0x2D4))
        return;
    if (Flag(a, 0x100, 0x2D4))
        return;
    if (MotionSquared(&a->base.secondaryMotion))
        return;
    if (context[0x6D0])
        return;
    if (*(u16 *)(context + 0x6D8))
        return;
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    a->base.tracking.target = (u32)(((u8 **)gPlayers)[0] + 0x24);
    a->base.field2C2 = Div(64, 3);
    if (MotionFlags(a, 0x80))
        OrFlags(&group->flags414, 4);
    if (MotionFlags(a, 0x80)) {
        SetState(a, 0, 0, 12);
        SetControl(a, group->entryControl41C, 0x1A);
        if (MotionFlags(a, 0x80)) {
            a->group.common.gate.counter += 300;
            if (a->group.common.gate.counter > a->group.common.gate.duration)
                a->group.common.gate.counter = a->group.common.gate.duration;
        }
    } else {
        SetState(a, 0, 0, 7);
        SetControl(a, a->base.entryTarget228, 1);
    }
    group->destination->pad = 1;
    if (!Chase(a, group, 0x200))
        Callback_InvokeActor(a, group->failure);
    {
        u16 *p = (void *)(context + 0x6D8);
        u32 value = 360;
        *p = value;
    }
}
