// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"

#ifdef NONMATCHING
// fuzzy: 77.8%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: address lifetimes, state scheduling, and register allocation.
u8 Skeleton_RerouteToTarget(void *);
u8 SceneActor_DispatchContextAction(void *, u32);
void Vorn_ResumeRouteAfterDelay(void);
void Sound_PlaySong(u32);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
static inline s32 Quarter(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}
static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline u32 Test(u16 *f, u32 m)
{
    u32 mask = m;
    return *f & mask;
}
static inline u8 ContextFlags(u8 *ctx)
{
    if (*(u16 *)(ctx + 0x6D4) & 0x1D)
        return 1;
    return 0;
}
static inline u32 Random4(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)gRandomTable + index);
    v &= 3;
    return v + 1;
}
// confidence=low: converges stagger motion before selecting delayed recovery or pursuit.
#define Vorn_UpdateStaggerRecovery sub_080AD75C
static inline void Chase_SetCtl_(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    {
        typeof(actor->field2B2) *q1 = &actor->field2B2;
        *q1 = 1;
    }
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline u32 Arg1(u32 k_, u8 *anim)
{
    return anim + k_;
}

inline static u8 ContextFlags_080AD75C(u8 *ctx)
{
    {
        u8 k1 = 0x1D;
        if ((*((u16 *)(ctx + 0x6D4))) & k1)
            return 1;
    }
    return 0;
}

void Vorn_UpdateStaggerRecovery(struct SceneActorRegistryResetView *actor, s32 tick)
{
    u8 *ctx = actor->context;
    u8 *anim = ((u8 *)actor->drawable) + 0x20;
    register u8 *group asm("sl") = (u8 *)(&actor->group);
    struct ContactPosition range;
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    register u32 *action asm("sb");
    u16 *flags;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        flags = &actor->flags2D4;
        if (Test(flags, 8)) {
            Sound_PlaySong(0x1CB);
            Sound_PlaySong(0x12A);
        }
        {
            s32 mask = ~1;
            *flags = mask & (*flags);
        }
        actor->field2B3 = 0;
    }
    action = &actor->flags2CC;
    {
        u32 mask = 0x2000;
        *action = mask | (*action);
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 = mask | actor->flags2D0;
    }
    if (tick <= 17)
        *((u16 *)Arg1(12, anim)) = 0;
    if ((Mod(tick, 12) == 0) && Test(&actor->flags2D4, 8)) {
        range.x = 60;
        range.y = 60;
        range.z = 40;
        SparkParticle_Spawn(&actor->position, Random4(), &range, actor->direction + 128, 10, 6, 2,
                            2);
    }
    {
        u32 offset = 0x310;
        s32 current = *((s16 *)(((u8 *)actor) + offset));
        u32 priorOffset = 0x318;
        s16 *field = (void *)(((u8 *)actor) + priorOffset);
        s32 difference = current - (*field);
        priorX = field;
        x = Quarter(difference);
    }
    {
        u32 offset = 0x314;
        s32 current = *((s16 *)(((u8 *)actor) + offset));
        u32 priorOffset = 0x31C;
        s16 *field = (void *)(((u8 *)actor) + priorOffset);
        s32 difference = current - (*field);
        priorZ = field;
        z = Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->motion.x += x;
    actor->motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z) && actor->field2B3) {
        actor->field2B0 = 1;
        flags = &actor->flags2D4;
        {
            s32 mask = ~0x120;
            *flags = mask & (*flags);
        }
        *action &= ~0x80;
        if (actor->field2D8 & 0x80000) {
            actor->flags2D6 |= 2;
            return;
        }
        if (Test(flags, 8)) {
            Chase_SetState(actor, 3, 0, 0);
            Chase_SetCtl_(actor, Vorn_ResumeRouteAfterDelay, 20);
            {
                s32 mask = ~8;
                *flags = mask & (*flags);
            }
            actor->field2C2 = 64;
        } else if (ContextFlags_080AD75C(ctx))
            SceneActor_DispatchContextAction(ctx, 1);
        else {
            actor->fieldDC = ((u32)gPlayers[0]) + 0x24;
            *action |= 16;
            (*((struct ContactPosition **)(group + 0x200)))->pad = 0;
            if ((!Skeleton_RerouteToTarget(actor)) &&
                (!Chase_TryRoute(actor, *((void **)(group + 0x1FC)), &actor->position,
                                 (void *)(actor->fieldDC + 12)))) {
                Callback_InvokeActor(actor, *((void **)(group + 0x224)));
                return;
            }
            Chase_SetCtl_(actor, actor->entryTarget228, 1);
            Chase_SetBehavior(actor, 7);
            actor->field2C2 = 32;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080AD75C);
#endif
