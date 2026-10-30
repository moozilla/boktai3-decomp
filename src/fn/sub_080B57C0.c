#include "global.h"
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

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u8 Chase(struct SceneActorRegistryResetView *actor, struct ApproachGroup *group,
                       u32 bits)
{
    void *route = group->routeObject;
    struct ContactPosition *position = &actor->body.position;
    u8 *target = *(u8 **)&actor->tracking.target;
    struct ContactPosition *destination = (void *)(target + 12);
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            actor->flags2D4 &= ~bits;
        }
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        } else {
            {
                actor->flags2D4 &= ~bits;
            }
            return 0;
        }
    }
}

struct SkeletonRouteContext {
    u8 unknown0[0x6D0];
    u8 active;
};
static inline u16 MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 value = actor->flags2D4;
    value &= mask;
    return value;
}
static inline void OrFlags(u16 *flags, u32 mask)
{
    u32 value = mask;
    value |= *flags;
    *flags = value;
}

static inline u32 Flag(struct SceneActorRegistryResetView *a, u32 m, u32 off)
{
    return off == 0x2D4 ? *(u16 *)((u8 *)a + off) & m : *(u32 *)((u8 *)a + off) & m;
}
static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a && (Flag(a, 0x40, 0x2D4) || Flag(a, 0x20000, 0x2D0) || a->health <= 0))
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
void Skeleton_BeginStationaryPursuit(struct SceneActorRegistryResetView *a)
{
    u8 *context;
    struct ApproachGroup *group;
    if (Unavailable(a))
        return;
    context = a->context;
    group = (void *)&a->group;
    if (Pending(context))
        return;
    if (Flag(a, 0xA03F, 0x2D4))
        return;
    if (Flag(a, 0x100, 0x2D4))
        return;
    if (MotionSquared(&a->secondaryMotion))
        return;
    if (context[0x6D0])
        return;
    if (*(u16 *)(context + 0x6D8))
        return;
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    a->tracking.target = (u32)(((u8 **)gPlayers)[0] + 0x24);
    a->field2C2 = Div(64, 3);
    if (MotionFlags(a, 0x80))
        OrFlags(&group->flags414, 4);
    if (MotionFlags(a, 0x80)) {
        SetState(a, 0, 0, 12);
        SetControl(a, group->entryControl41C, 0x1A);
        if (MotionFlags(a, 0x80)) {
            a->group.field418 += 300;
            if (a->group.field418 > a->group.field416)
                a->group.field418 = a->group.field416;
        }
    } else {
        SetState(a, 0, 0, 7);
        SetControl(a, a->entryTarget228, 1);
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
