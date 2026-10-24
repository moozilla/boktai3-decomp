#include "scene_actor_reset.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: address lifetimes, register allocation, and instruction scheduling.
// MATCHING_CONCESSION: register-pinning
// confidence=low: eases landing displacement, then restores route pursuit.
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void ScaledShadow_Enable(void *);
void ScaledShadow_Destroy(void *);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
struct SpiderFlagWord {
    s16 bits : 16;
};
struct SpiderRecoveryContext {
    u8 pad[0x62C];
    struct SpiderFlagWord flags;
};
struct SpiderHalfFlags {
    s16 bits : 16;
};
static inline u8 TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void ClearHidden(struct SpiderHalfFlags *p, u32 mask)
{
    p->bits &= mask;
}
struct RecoveryPacket {
    u8 pad[8];
    u8 state, phase, behavior, gap;
    u32 delay;
    void *callback;
};
static inline void SetCollider(struct ContactCollider *c)
{
    c->flags |= 4;
}
static inline void ClearCollider(struct ContactCollider *c)
{
    u32 mask = ~4;
    c->flags &= mask;
}
static inline void ClearContext(struct SpiderFlagWord *p, u32 mask)
{
    p->bits &= mask;
}
static inline void SaveRecovery(struct RecoveryPacket *p, void *callback, u8 behavior, u32 delay)
{
    p->delay = delay;
    p->callback = callback;
    p->state = 0;
    p->phase = 0;
    p->behavior = behavior;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 kind)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = 0;
    {
        u32 *p = &a->flags114;
        u32 mask = ~1;
        *p &= mask;
    }
    a->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u8 state, u8 phase, u8 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

void sub_080A4798(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
struct LandingState {
    u8 pad[0x310];
    s16 targetX, gapX, targetZ, gapZ, currentX, gapCX, currentZ;
};
struct LandingSequence {
    u8 pad[8];
    u16 frame;
    u8 gap[4];
    u16 elapsed;
};
static inline s32 Quarter(s32 x)
{
    if (x >= 0)
        return x >> 2;
    return -((-x) >> 2);
}
static inline u8 MoveLanding(struct SceneActorRegistryResetView *a)
{
    struct LandingState *s = (void *)a;
    s16 *cx;
    s16 *cz;
    s32 x = Quarter(s->targetX - s->currentX);
    s32 z;
    cx = &s->currentX;
    z = Quarter(s->targetZ - s->currentZ);
    cz = &s->currentZ;
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    a->motion.x += x;
    a->motion.z += z;
    *cx += x;
    *cz += z;
    if (!x && !z)
        return 1;
    return 0;
}
static inline void AddMotion(struct SceneActorRegistryResetView *a, u32 mask)
{
    register u32 m asm("r0") = mask;
    asm("" : "+r"(m));
    a->flags2D4 = m | a->flags2D4;
}
static inline void AddGroup(struct SceneActorRegistryControlGroup *g, u32 mask)
{
    g->flags414 = mask | g->flags414;
}
static inline void ClearFlag(u16 *p, u32 mask)
{
    *p &= mask;
}
inline static void ClearFlag_1(struct SceneActorRegistryResetView *a_, u32 mask)
{
    u16 *p = &a_->flags2D4;
    *p &= mask;
}

inline static u8 MoveLanding_1(struct LandingState *s, struct SceneActorRegistryResetView *a)
{
    s16 *cx;
    s16 *cz;
    s32 x = Quarter(s->targetX - s->currentX);
    s32 z;
    cx = &s->currentX;
    z = Quarter(s->targetZ - s->currentZ);
    cz = &s->currentZ;
    if (x > 200)
        x = 200;
    else if (x < (-200))
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < (-200))
        z = -200;
    a->motion.x += x;
    a->motion.z += z;
    *cx += x;
    *cz += z;
    if ((!x) && (!z))
        return 1;
    return 0;
}

void Spider_UpdateLandingMotion(struct SceneActorRegistryResetView *a, s32 frame)
{
    struct LandingSequence *seq = (void *)(((u8 *)a->drawable) + 0x48);
    u32 *flags;
    if (TakeEntry(&a->field2B2)) {
        ClearContext(&((struct SpiderRecoveryContext *)a->context)->flags, ~6);
        SceneActorAnimation_SelectDirectionalEntry(a, 3);
        AddMotion(a, 0x20);
    }
    {
        u32 mask = 0x10000;
        u32 *p = &a->flags2D0;
        *p |= mask;
    }
    {
        u32 mask = 0x2000;
        flags = &a->flags2CC;
        *flags |= mask;
    }
    if ((frame <= 17) && (seq->frame == 1))
        seq->elapsed = 0;
    if (MoveLanding_1((void *)a, a)) {
        u8 *group = (u8 *)(&a->group);
        do {
            a->field2B0 = 1;
        } while (0);
        SetState(a, 0, 0, 12);
        SetControl(a, *((void **)(group + 0x48)), 26);
        sub_080A4798(a);
        SceneActorMovement_ResolveRoutePosition(a);
        AddGroup(&a->group, 4);
        AddMotion(a, 0x80);
        a->group.field418 = a->group.field416;
        ClearFlag_1(a, ~0x20);
        a->direction = a->fieldE9;
        {
            u32 mask = 0x10;
            *flags |= mask;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Spider_UpdateLandingMotion);
#endif
