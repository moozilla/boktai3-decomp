#include "global.h"
#include "scene_actor_control_phase.h"

#ifdef NONMATCHING
// twin.py transplant from a matched near-clone
// Remaining differences: register allocation, address reuse, and instruction order.
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))
void SceneActorMovement_ResolveRoutePosition(void *);

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
    if (F(c, 0x62c, u16) & m)
        return 1;
    return 0;
}

static inline u32 Test32(u32 *p, u32 m)
{
    return *p & m;
}

extern u8 *gUnk_02000580[];
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
    s32 direction = a->direction;
    s32 difference = Math_Atan2(delta->x, delta->z) - direction;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    return difference;
}
static inline void BeginContact(struct SceneActorRegistryResetView *a, u8 *nav)
{
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
        u16 *p = &a->flags2D4;
        u32 m = 128;
        u32 v = *p;
        m |= v;
        *p = m;
    }
    {
        u32 v = F(a, 0x416, u16);
        F(a, 0x418, u16) = v;
    }
}
// confidence=low: detects a player in front and selects close or distant contact behavior
u8 SceneActorPlacement_SelectPlayerApproach(struct SceneActorRegistryResetView *a)
{
    u8 *transform = ((u8 *)a) + 0x50;
    u8 *c = (void *)SceneActorControl_GetState(a);
    struct ContactPosition delta;
    u8 *player;
    struct ContactPosition *pos;
    s32 height;
    s32 angle;
    u32 distance;
    if (!HasFlag(c, 2)) {
        if (SceneActorPlacement_BeginContactRoute(a))
            return 1;
        if (Test32((void *)(gUnk_02000580[0] + 0x20), 0x20000))
            return 0;
    } else if (SceneActorPlacement_BeginContactRoute(a))
        return 1;
    player = gUnk_02000580[0];
    if (!player)
        return 0;
    asm("" : : "r"(player));
    pos = (void *)(player + 0x30);
    Delta(&delta, &a->position, pos);
    height = delta.y;
    if (height < 0)
        height = -height;
    if (height > 255)
        return 0;
    distance = (delta.x * delta.x) + (delta.z * delta.z);
    Math_Atan2(delta.x, delta.z);
    if (distance >= (a->distanceLimitF8 * 2))
        return 0;
    Delta(&delta, &a->position, pos);
    angle = Facing(a, &delta);
    if (angle < 0)
        angle = -angle;
    if (angle > 32)
        return 0;
    if (!sub_0821D4F0(transform + 12, gUnk_02000580[0] + 0x30, 5, 256))
        return 0;
    a->fieldDC = (u32)(player + 0x24);
    if (((s32)distance) <= 0x3fff)
        BeginContact(a, ((u8 *)a) + 0x3d4);
    else {
        State(a, 16);
        Control(a, sub_080A28A8, 3);
    }
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorPlacement_SelectPlayerApproach);
#endif
