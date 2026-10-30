#include "global.h"
#include "scene_actor_control_phase.h"
#include "player.h"

void SceneActorMovement_ResolveRoutePosition(void *);

struct SpiderApproachContext {
    u8 unknown0[0x62C];
    u16 flags;
};
struct SpiderApproachPlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0xC];
    struct ContactPosition position;
};
static inline void Control(struct SceneActorRegistryResetView *a, void *c, u32 k)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = k;
    a->target = c;
    a->field2C8 = 0;
    {
        u32 *p = &a->flags114;
        u32 m = ~1;
        *p &= m;
    }
    a->word104 = 0;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 k)
{
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = k;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline u8 HasFlag(u8 *c, u32 m)
{
    if (((struct SpiderApproachContext *)c)->flags & m)
        return 1;
    return 0;
}

static inline u32 Test32(u32 *p, u32 m)
{
    return *p & m;
}

u8 SceneActorPlacement_BeginContactRoute(void *);
s32 Math_Atan2(s32, s32);
u8 sub_0821D4F0(void *, void *, u32, u32);
void sub_080A4798(void *);
void sub_080A28A8(void);
static inline void Delta(struct ContactPosition *out, struct ContactPosition *from,
                         struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline s32 Facing(struct SceneActorRegistryResetView *a, struct ContactPosition *delta)
{
    s32 direction = a->body.direction;
    s32 difference = Math_Atan2(delta->x, delta->z) - direction;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    return difference;
}
static inline void BeginContact(struct SceneActorRegistryResetView *a,
                                struct SceneActorRegistryControlGroup *nav)
{
    State(a, 12);
    Control(a, nav->entryControl41C, 26);
    sub_080A4798(a);
    SceneActorMovement_ResolveRoutePosition(a);
    {
        u16 *p = &a->group.flags414;
        u32 m = 4;
        u32 v = *p;
        m |= v;
        *p = m;
    }
    {
        u32 m = 128;
        u16 *p = &a->flags2D4;
        u32 v = *p;
        m |= v;
        *p = m;
    }
    {
        u32 v = a->group.field416;
        a->group.field418 = v;
    }
}
// confidence=low: detects a player in front and selects close or distant contact behavior
u8 SceneActorPlacement_SelectPlayerApproach(struct SceneActorRegistryResetView *a)
{
    u8 *transform = (u8 *)&a->body.id;
    u8 *c = (void *)SceneActorControl_GetState(a);
    struct ContactPosition delta;
    u8 *player;
    u8 *candidate;
    s32 height;
    s32 angle;
    u32 distance;
    if (!HasFlag(c, 2)) {
        if (SceneActorPlacement_BeginContactRoute(a))
            return 1;
        if (Test32(&((struct SpiderApproachPlayer *)gPlayers[0])->flags, 0x20000))
            return 0;
    } else if (SceneActorPlacement_BeginContactRoute(a))
        return 1;
    candidate = gPlayers[0];
    if (!candidate)
        return 0;
    player = candidate;
    Delta(&delta, &a->body.position, &((struct SpiderApproachPlayer *)player)->position);
    height = delta.y;
    if (height < 0)
        height = -height;
    if (height > 255)
        return 0;
    distance = (delta.x * delta.x) + (delta.z * delta.z);
    Math_Atan2(delta.x, delta.z);
    if (distance >= (a->tracking.limit * 2))
        return 0;
    Delta(&delta, &a->body.position, &((struct SpiderApproachPlayer *)player)->position);
    angle = Facing(a, &delta);
    if (angle < 0)
        angle = -angle;
    if (angle > 32)
        return 0;
    if (!sub_0821D4F0(transform + 12, &((struct SpiderApproachPlayer *)player)->position, 5, 256))
        return 0;
    a->tracking.target = (u32)((struct SpiderApproachPlayer *)player)->unknown24;
    if (((s32)distance) <= 0x3fff)
        BeginContact(a, &a->group);
    else {
        State(a, 16);
        Control(a, sub_080A28A8, 3);
    }
    return 1;
}
