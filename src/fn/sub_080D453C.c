#include "global.h"
// confidence=low: consumes a nearby stimulus and attempts a tagged-position route.
#define Mummy_TryStimulusRoute sub_080D453C
#include "scene_actor_chase.h"
u8 SceneStimulus_FindOverlap(s16, s16, u16, u8, u16);
void SceneActorPosition_ResolveTaggedPosition(void *, void *, void *, s8);
u8 SceneActor_TryTrackNearbyPlayer(void *, void *, u32);
void SceneGrid_SnapPositionToCellCenter(void *, u32);
void SceneActorAttachment_RetargetNearestPlayer(void *, void *);
struct MummyStimulusInfo {
    u16 field4C, radius;
    u32 field50;
    u16 timer : 16;
    u8 phase[1], kind;
};
struct MummyStimulusRouteLink {
    u8 unknown0[8];
    u16 *point;
};
struct MummyStimulusGroup {
    u8 unknown0[0x40];
    u16 flags, timerLimit, movementTimer;
    u8 unknown46[6];
    struct MummyStimulusInfo stimulus;
    struct ContactPosition target;
    u8 unknown60[4];
    u32 cell;
    void *control;
    u8 unknown6C[8];
    struct MummyStimulusRouteLink link;
    u8 unknown80[0x17C];
    void *route;
    struct ContactPosition *destination;
    u8 unknown204[0x20];
    void *fallback;
};
struct MummyStimulusContext {
    u8 unknown0[0x6EA];
    u8 phase[1];
    u8 unknown6EB[7];
    u16 flags;
};
static inline u8 Mummy_TryRoute(u16 *flags, u32 mask, void *route, struct ContactPosition *from,
                                struct ContactPosition *to)
{
    s32 result = Nav_IsPositionInMesh(route, from, to);
    if (!result) {
        *flags &= ~mask;
        return 0;
    }
    result = Terrain_ApplyBlocked(route, from, to);
    if (result) {
        if (result != 2)
            Memory_Clear(from + 1, 8);
        return 1;
    }
    *flags &= ~mask;
    return 0;
}
static inline u8 Mummy_TestContactFlags(struct SceneActorReactionMotion *motion, u32 mask)
{
    if (motion->flags & mask)
        return 1;
    return 0;
}
static inline u16 KeepMotionFlags(struct SceneActorRegistryResetView *a, u32 mask, u16 **out)
{
    u16 *flags = &a->flags2D4;
    u16 value = *flags;
    value &= mask;
    *out = flags;
    return value;
}
static inline u32 ActorWordBits(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2CC & mask;
}
static inline u16 ActorHalfBits(struct SceneActorRegistryResetView *a, u32 mask)
{
    u16 value = a->flags2D4;
    value &= mask;
    return value;
}
static inline u32 ClearActivity(u32 *flags, u32 mask)
{
    u32 value = *flags;
    value &= mask;
    *flags = value;
    return value;
}
static inline u8 Mummy_ShouldSkipStimulus(u8 *object)
{
    struct SceneActorRegistryResetView *a = (void *)object;
    u32 motion;
    motion = ClearActivity(&a->flags2D0, 0xFEFFFFFF);
    if (!ActorWordBits(a, 0x200000))
        return 1;
    if (motion & 4)
        return 1;
    if (ActorWordBits(a, 0x100) || ActorHalfBits(a, 0xE13F) ||
        Mummy_TestContactFlags((void *)&a->tracking.target, 16))
        return 1;
    if (((struct SceneActorRegistryResetView *)object)->field2B8)
        return 1;
    {
        u16 *timer = &((struct SceneActorRegistryResetView *)object)->colliderA.responseValue;
        object = (u8 *)timer;
        if (*timer)
            return 1;
    }
    return 0;
}
u32 Mummy_TryStimulusRoute(struct SceneActorRegistryResetView *a)
{
    struct MummyStimulusGroup *group;
    struct MummyStimulusContext *body;
    s8 tag;
    u32 started;
    u16 *motionFlags;
    if (Mummy_ShouldSkipStimulus((u8 *)a))
        goto failed;
    group = (void *)&a->group;
    ((struct MummyStimulusGroup *)&a->group)->stimulus.phase[0] = 0;
    started = 0;
    body = a->context;
    tag = SceneStimulus_FindOverlap(
        a->body.position.x, a->body.position.z, ((struct MummyStimulusGroup *)&a->group)->stimulus.radius,
        ((struct MummyStimulusGroup *)&a->group)->stimulus.kind, (u16)a->body.id);
    if (tag < 0)
        goto failed;
    if (!((struct MummyStimulusGroup *)&a->group)->stimulus.timer) {
        ((struct MummyStimulusGroup *)&a->group)->stimulus.timer = 180;
        Chase_SetCtl(a, group->control, 9);
        Chase_SetBehavior(a, 11);
        {
            u32 mask = 0x4000;
            body->flags = mask | (body->flags);
        }
        started = 1;
    }
    if (KeepMotionFlags(a, 0x80, &motionFlags)) {
        u16 *timer = &((struct MummyStimulusGroup *)&a->group)->movementTimer;
        *timer += 300;
        if ((*timer) > (((struct MummyStimulusGroup *)&a->group)->timerLimit))
            *timer = ((struct MummyStimulusGroup *)&a->group)->timerLimit;
    }
    SceneActorPosition_ResolveTaggedPosition(&group->stimulus, &a->body.id, group->route, tag);
    if (SceneActor_TryTrackNearbyPlayer(a, &group->target, group->cell))
        goto success;
    Chase_SnapToRoute(group->destination, (u8 *)&group->link);
    (group->destination)->y = a->body.position.y;
    (group->destination)->pad = 0;
    SceneGrid_SnapPositionToCellCenter(&group->target, group->cell);
    if (!Mummy_TryRoute(motionFlags, 0x200, group->route, &a->body.position, &group->target)) {
        Callback_InvokeActor(a, group->fallback);
    success:
        return 1;
    }
    {
        u32 mask = 16;
        a->flags2CC |= mask;
    }
    a->field2C2 = Div(64, 3);
    {
        u32 mask = 0xFF7FFFFF;
        a->flags2CC = mask & a->flags2CC;
    }
    body->phase[0] = 4;
    {
        u32 mask = 0x200;
        *motionFlags = mask | *motionFlags;
    }
    {
        u32 mask = 0x1000000;
        a->flags2D0 |= mask;
    }
    SceneActorAttachment_RetargetNearestPlayer(a, &group->target);
    if (started)
        goto success;
failed:
    return 0;
}
