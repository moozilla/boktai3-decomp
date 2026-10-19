#include "global.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_helpers.h"
#include "scene_actor_route.h"
#include "scene_actor_terrain.h"

extern u8 gUnk_08605E34[];

void Lun_InitMotionPreset(u8 *p)
{
    *(u8 **)(p + 0x284) = gUnk_08605E34;
}

void SceneActorControl_OnIdle(struct SceneActorRegistryResetView *);

INCLUDE_ASM("asm/nonmatching", sub_0809C834);

INCLUDE_ASM("asm/nonmatching", sub_0809C91C);

void Lun_UpdateSway(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    if (state->sway > 320) {
        state->swayStep = -1;
        state->sway = 320;
    } else if (state->sway < -320) {
        state->swayStep = 1;
        state->sway = -320;
    }
    state->sway += state->swayStep * 12;
}

extern u32 gUnk_03005308;

s32 Mod(s32, s32);

u8 *Path_GetPointData(struct SceneActorRouteDefinition *);

static inline u32 Random(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(u16 *)(offset + (u32)gUnk_0203B400);
}

void Lun_SelectRandomRouteEntry(struct SceneActorRouteCursor *p)
{
    struct SceneActorRouteCursor *q = p;
    u8 n;
    n = Mod(Random(), p->definition->pointCount);
    if (n == p->pointIndex)
        n = Mod(n + 1, p->definition->pointCount);
    q->pointIndex = n;
    q->unknown3 = 0;
    q->point = (struct SceneActorRoutePoint *)(Path_GetPointData(q->definition) + q->pointIndex * 8);
}

void Lun_OnBodyContact(void);

void sub_0809AE0C(void);

void SceneActorCollision_InitBody(void *, void *, void *, void *, void *);

void Collider_Init(void *, u16, u32, u32, u32, void *, void *);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void Collider_SetPosition(void *, void *, u32);

void Collider_SetCallback(void *, void *, void *);

void ActorContact_InitializeBodyAndSecondary(struct SceneActorRegistryResetView *a)
{
    struct ContactPackedVector bodySize, bodyOffset, secondarySize, secondaryOffset;
    struct ContactCollider *collider;
    bodySize.x = 0x10;
    bodySize.y = 0x80;
    bodySize.z = 0x10;
    bodyOffset.x = 0;
    bodyOffset.y = 0;
    bodyOffset.z = 0;
    SceneActorCollision_InitBody(a, &bodySize, &bodyOffset, Lun_OnBodyContact, a);
    secondarySize.x = 0x10;
    secondarySize.y = 0x54;
    secondarySize.z = 0x10;
    secondaryOffset.x = 0;
    secondaryOffset.y = 0;
    secondaryOffset.z = 0;
    {
        u32 flags = 0x2003;
        collider = &a->colliderB;
        Collider_Init(collider, a->id, flags, 0, 0x10, &secondarySize, &secondaryOffset);
    }
    Collider_SetAttackProperties(collider, 0, 0, 0, 0, 0);
    Collider_SetPosition(collider, &a->position, 0);
    Collider_SetCallback(collider, sub_0809AE0C, a);
}

INCLUDE_ASM("asm/nonmatching", sub_0809CC8C);

typedef struct {
    u32 a, b;
} P;

void Lun_SetMovementEndpoints(u8 *p, P *q)
{
    u8 *r = *(u8 **)(p + 0x3d0);
    *(P *)(r + 0x414) = *(P *)(p + 0x5c);
    *(P *)(r + 0x41c) = *q;
    *(u16 *)(r + 0x3e0) = 0;
}

INCLUDE_ASM("asm/nonmatching", sub_0809CCF4);

void Lun_InitProfile(u8 *p)
{
    *(u32 *)(p + 0x308) = SceneActorProfiles_FindOrAdd(0x11, 0);
}

INCLUDE_ASM("asm/nonmatching", sub_0809CEBC);

void SceneActorControl_ResetCollisionFlags(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u32 *state = (void *)SceneActorControl_GetMotionState(actor);
    u32 mask = 0x100000;
    u32 *flags = &actor->flags2D0;
    *flags |= mask;
    {
        u32 offset = 0x3D4;
        u32 mask;
        state = (void *)((u8 *)state + offset);
        mask = ~1;
        *state &= mask;
    }
    {
        struct ContactCollider *colliderB = &actor->colliderB;
        u32 mask;
        mask = ~4;
        colliderB->flags = mask & colliderB->flags;
        {
            struct ContactCollider *colliderA = &actor->colliderA;
            mask &= colliderA->flags;
            colliderA->flags = mask;
        }
    }
    {
        u16 *p = &actor->flags2C0;
        u32 mask;
        mask = 0xFFFFF7FF;
        *p &= mask;
    }
}

void Lun_GetDirectionalFrame(u16 a, s32 *b, s32 *c)
{
    s32 t = ((a + 0x70) >> 5) & 7;
    if (t > 4) {
        *b = 8 - t;
        *c = 1;
    } else {
        *b = t;
        *c = 0;
    }
}

void sub_0809CCF4(u8 *);

void CpuSet(const void *, void *, u32);

static inline void Fill(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}

void Lun_BeginMoveToPosition(u8 *s, s16 *v)
{
    u8 *p = *(u8 **)(s + 0x3d0);
    u32 stack[3];
    s16 *a;
    s16 *b;
    Lun_SetMovementEndpoints(s, v);
    sub_0809CCF4(s);
    {
        u32 one = 1;
        u8 *q = s + 0x2a8;
        u32 zero = 0;
        *q = zero;
        {
            s32 off = 0x2a9;
            s[off] = zero;
            s[0x2aa] = one;
            *(u32 *)(s + off + 3) = zero;
        }
        s[0x2b1] = one;
        b = (s16 *)(stack + 1);
        a = (s16 *)(s + 0x5c);
        b[0] = v[0] - a[0];
        b[1] = v[1] - a[1];
        b[2] = v[2] - a[2];
        s[0x3cd] = Math_Atan2(b[0], b[2]);
        Fill(p + 0x424, stack, zero);
    }
}

struct SceneActorTargetLink {
    u8 *target;
    u8 unknownE0[0xB];
    u8 flags;
};

void SceneActorControl_ResetTargetFlags(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *s = input;
    struct SceneActorControlState *q = SceneActorControl_GetState(s);
    u16 *hp = &s->flags2D4;
    u32 hm = 0xFFFFFEFF;
    struct SceneActorTargetLink *t;
    u32 bm;
    u16 zero;
    u16 hv;
    hv = *hp & hm;
    zero = 0;
    *hp = hv;
    t = (void *)((u8 *)s + 0xDC);
    bm = ~8;
    t->flags = t->flags & bm;
    *(u16 *)((u8 *)q + 0x444) = zero;
}

#define RouteCursor_SelectRandomPoint Lun_SelectRandomRouteEntry

#define RouteActor_SetDestination Lun_BeginMoveToPosition

struct RouteActorDestinationState {
    u8 unknown0[0x42C];
    struct SceneActorRouteCursor route;
    u8 unknown438[0xA];
    u16 delayDuration;
    u16 delay;
};

static inline u16 TestFlag(struct SceneActorRegistryActor *p, u32 mask)
{
    mask &= p->flags2D4;
    return mask;
}

void RouteActor_UpdateRandomDestination(struct SceneActorRegistryActor *actor)
{
    struct RouteActorDestinationState *state = actor->context;
    if (!TestFlag(actor, 0x100)) {
        state->delay = 0;
    } else if (state->delay) {
        state->delay--;
    } else {
        struct ContactPosition target;
        struct ContactPosition *p;
        struct SceneActorRouteCursor *route = &state->route;
        s32 cell;
        u16 attr;
        u32 kind;
        s32 height;
        Lun_SelectRandomRouteEntry(route);
        p = &target;
        p->x = route->point->x;
        p->z = route->point->z;
        cell = SceneActorTerrain_GetCellIndex(p);
        attr = *SceneActorTerrain_GetCell(cell);
        kind = attr >> 4;
        {
            u32 mask = 15;
            mask &= attr;
            height = mask << 8;
        }
        switch (kind) {
            case 1:
                height -= ((u8 *)p)[4];
                break;
            case 2:
                height -= ((u8 *)p)[0];
                break;
        }
        p->y = height;
        Lun_BeginMoveToPosition((u8 *)actor, (s16 *)&target);
        SceneActorControl_ResetTargetFlags((u8 *)actor);
    }
}

INCLUDE_ASM("asm/nonmatching", sub_0809D2A8);

extern u8 *gUnk_02000580[4];

static inline u8 TestTargetCheckBit(u8 *s)
{
    u8 *p = s + 0xDC;
    u32 mask = 2;
    if (p[15] & mask)
        return 1;
    return 0;
}

u32 Lun_OnTargetCheck(u8 *s)
{
    u8 *p;
    s32 distance;
    if (!TestTargetCheckBit(s))
        goto zero;
    p = s + 0xDC;
    {
        s32 mask = -3;
        mask &= p[15];
        p[15] = mask;
    }
    distance = *(s16 *)(s + 0x5E) - *(s16 *)(gUnk_02000580[0] + 0x32);
    if (distance < 0)
        distance = -distance;
    if (distance > 0x200)
        goto zero;
    {
        u32 *a = (u32 *)(s + 0xF0);
        u32 *b = (u32 *)(s + 0xF8);
        if (*a >= *b)
            goto zero;
    }
    goto one;
zero:
    return 0;
one: {
    u32 mask = 0x80;
    u16 *flags = (u16 *)(s + 0x2D6);
    mask |= *flags;
    *flags = mask;
}
    {
        u32 mask = 0x10;
        mask |= p[15];
        p[15] = mask;
    }
    return 1;
}

void sub_0809D424(struct SceneActorRegistryResetView *actor)
{
    struct RouteActorDestinationState *state = actor->context;
    u16 mask = 0x100;
    u16 flag16 = actor->flags2D4 & mask;
    u32 flag = flag16;
    if (flag) {
        state->delay = state->delayDuration;
    } else {
        void *callback = SceneActorControl_OnIdle;
        u32 kind = 0x10;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 fmask = ~1;
            *flags &= fmask;
        }
        actor->word104 = 0;
        actor->state2A8 = 0;
        actor->phase2A9 = 1;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        {
            u8 *p = (u8 *)actor + 0xDC;
            u32 bit = 8;
            bit |= p[0xF];
            p[0xF] = bit;
        }
    }
}

static inline u8 TestMask(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline u8 LiftFlag(struct SceneActorControlMotionState *state)
{
    u32 mask = 2;
    u32 offset = 0x3D4;
    return TestMask(*(u32 *)((u8 *)state + offset), mask);
}

void SceneActorControl_ConsumeLiftFlagPenalty(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    if (!LiftFlag((void *)SceneActorControl_GetMotionState(actor)))
        return;
    if (actor->paletteOffset == 0) {
        u8 *p = gUnk_02000580[0] + 0x4F6;
        *p = *p - 1;
    }
    {
        u32 *flags = (void *)((u8 *)SceneActorControl_GetMotionState(actor) + 0x3D4);
        u32 clearMask = ~2;
        *flags &= clearMask;
    }
}

void sub_0809C91C(u8 *);

u32 Lun_UpdateMovement(u8 *p)
{
    sub_0809C91C(p);
    RouteActor_UpdateRandomDestination(p);
    return 1;
}

void sub_0809D2A8(void);

u32 Lun_UpdateTerrainBounce(u8 *p)
{
    sub_0809D2A8();
    return 1;
}

INCLUDE_ASM("asm/nonmatching", sub_0809D574);

INCLUDE_ASM("asm/nonmatching", sub_0809D7C4);

extern u8 gUnk_08605E68[];

void Lun_InitSecondaryActionTable(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08605E68;
}

#define gPlayerSlots              gUnk_02000580

void sub_0809B6AC(void);

static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline u16 SetFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 *flags = &actor->flags2D4;
    return *flags = mask | *flags;
}

void RouteActor_UpdatePlayerDestinationPhase(struct SceneActorRegistryResetView *actor)
{
    if (TakeFlag(&actor->field2B1)) {
        struct RouteActorDestinationState *state = actor->context;
        void *callback = sub_0809B6AC;
        u32 kind = 10;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        SetFlag(actor, 0x100);
        state->delay = state->delayDuration;
    }
    if (actor->field2B0)
        Lun_BeginMoveToPosition(actor, gUnk_02000580[0] + 0x30);
    else
        actor->counter2AC++;
}

INCLUDE_ASM("asm/nonmatching", sub_0809DA14);

extern u8 gUnk_08605E70[];

void Lun_InitReactionTable(u8 *p)
{
    *(u8 **)(p + 0x288) = gUnk_08605E70;
}

INCLUDE_ASM("asm/nonmatching", sub_0809DBAC);

u32 Lun_UpdateMovement(u8 *);

void SceneActorControl_DispatchControl(u8 *);

u32 Lun_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    Lun_UpdateMovement(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    Lun_UpdateTerrainBounce(p);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Lun_OnPlayerTargetInit(u8 *p)
{
    *(u8 **)(p + 0xdc) = gUnk_02000580[0] + 0x24;
    SceneActor_UpdateActiveCallbacks(p);
    return 0;
}

void Sprite_Remove(void *);

void Anim_Remove(void *);

void Heap_Free(void *);

void SceneMessages_UnregisterReceiver(void *);

void Shadow_Destroy(void *);

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

void Collider_Remove(void *);

void SceneActorRegistry_UnlinkActor(void *);

static inline u8 tst(u16 *a, u32 m)
{
    if (*a & m)
        return 1;
    return 0;
}

struct S {
    u8 f0[4];
    u8 *p4;
    u8 b8;
    u8 f1[0x2b7];
    u16 h2c0;
};

u32 Lun_Destroy(struct S *s)
{
    u8 *p = s->p4;
    u8 *q;
    u32 m;
    u32 v;
    if (s->b8 == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(p + 0x20);
    Heap_Free(p);
    m = 0x100;
    if (tst((u16 *)((u8 *)s + 0x2c0), m))
        SceneMessages_UnregisterReceiver((u8 *)s + 0xc);
    {
        u8 *r = (u8 *)s + 0x320;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = *(u32 *)((u8 *)s + 0x2d0) & m;
    q = (u8 *)s + 0x50;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove((u8 *)s + 0x134);
    SceneActorRegistry_UnlinkActor(s);
    return 0;
}

INCLUDE_ASM("asm/nonmatching", sub_0809DD78);

s32 sub_0809DD78(void *);

void Lun_Create(void)
{
    void *m = Heap_Alloc(0x44c);
    if (m != 0) {
        Memory_Clear(m, 0x44c);
        if (sub_0809DD78(m) < 0) {
            Lun_Destroy(m);
            Heap_Free(m);
        }
    }
}
