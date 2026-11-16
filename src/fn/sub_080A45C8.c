#include "global.h"
#include "scene_actor_control_phase.h"
#include "player.h"
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))
void SceneActorMovement_ResolveRoutePosition(void *);

static inline void Control(struct ChaseActor *a, void *c, u32 k)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = 0;
    a->base.field2B5 = k;
    a->base.target = c;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.flags114;
        u32 m = ~1;
        *p &= m;
    }
    a->base.word104 = 0;
}
static inline void State(struct ChaseActor *a, u32 k)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = k;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}
static inline u8 HasFlag(u8 *c, u32 m)
{
    if (F(c, 0x62c, u16) & m)
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
static inline void Delta(struct ContactPosition *out, struct ContactPosition *from,
                         struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline s32 Facing(struct ChaseActor *a, struct ContactPosition *delta)
{
    s32 direction = a->base.body.direction;
    s32 difference = Math_Atan2(delta->x, delta->z) - direction;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    return difference;
}

static inline u32 SaveFlags(struct ChaseActor *a, u16 **out, u32 m)
{
    u16 *p = &a->base.flags2D4;
    u32 r = *p & m;
    *out = p;
    return r;
}
// confidence=low: starts close player contact when within the actor's stored distance limit
u8 SceneActorPlacement_BeginNearbyPlayerContact(struct ChaseActor *a)
{
    u8 *c = (void *)SceneActorControl_GetState(a);
    u8 *nav = (u8 *)a + 0x3d4;
    u8 *transform = (u8 *)a + 0x50;
    u8 *player;
    u16 *flags;
    struct ContactPosition delta;
    s32 angle;
    if (HasFlag(c, 4))
        return 0;
    if (SceneActorPlacement_BeginContactRoute(a))
        return 1;
    player = ((u8 **)gPlayers)[0];
    if (Test32((void *)(player + 0x20), 0x20000))
        return 0;
    if (SaveFlags(a, &flags, 128))
        return 0;
    if (a->base.tracking.distance > a->base.tracking.limit)
        return 0;
    Delta(&delta, &a->base.body.position, (void *)(player + 0x30));
    angle = Facing(a, &delta);
    if (angle < 0)
        angle = -angle;
    if (angle > 32)
        return 0;
    if (!sub_0821D4F0(transform + 12, ((u8 **)gPlayers)[0] + 0x30, 5, 256))
        return 0;
    a->base.tracking.target = (u32)(((u8 **)gPlayers)[0] + 0x24);
    State(a, 12);
    Control(a, F(nav, 0x48, void *), 26);
    sub_080A4798(a);
    SceneActorMovement_ResolveRoutePosition(a);
    {
        u16 *p = (void *)((u8 *)a + 0x414);
        u32 m = 4;
        u32 v = *p;
        m |= v;
        *p = m;
    }
    {
        u32 m = 128;
        u32 v = *flags;
        m |= v;
        *flags = m;
    }
    {
        u32 v = F(a, 0x416, u16);
        F(a, 0x418, u16) = v;
    }
    return 1;
}
