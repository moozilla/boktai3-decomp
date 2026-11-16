#include "scene_actor_reset.h"

// confidence=low: eases landing displacement, then restores route pursuit.
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
struct SpiderFlagWord {
    s16 bits : 16;
};
struct SpiderRecoveryContext {
    u8 pad[0x62C];
    struct SpiderFlagWord flags;
};
static inline u8 Spider_TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void Spider_ClearContextFlags(struct SpiderFlagWord *p, u32 mask)
{
    p->bits &= mask;
}
static inline void Spider_SetControl(struct ChaseActor *a, void *callback, u32 kind)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = 0;
    a->base.field2B5 = kind;
    a->base.target = callback;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.flags114;
        u32 mask = ~1;
        *p &= mask;
    }
    a->base.word104 = 0;
}
static inline void Spider_SetState(struct ChaseActor *a, u8 state, u8 phase, u8 behavior)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
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
static inline s32 Spider_QuarterDisplacement(s32 x)
{
    if (x >= 0)
        x >>= 2;
    else
        x = -((-x) >> 2);
    return x;
}
static inline void Spider_AddMotionFlags(struct ChaseActor *a, u32 mask)
{
    u32 value = mask;
    value |= a->base.flags2D4;
    a->base.flags2D4 = value;
}
static inline void Spider_AddGroupFlags(struct SceneActorRegistryControlGroup *g, u32 mask)
{
    u16 *flags = &g->gate.flags;
    u32 value = mask;
    value |= *flags;
    *flags = value;
}
static inline void Spider_ClearMotionFlags(struct ChaseActor *a_, u32 mask)
{
    u16 *p = &a_->base.flags2D4;
    *p &= mask;
}

static inline u8 Spider_MoveLanding(struct LandingState *s, struct ChaseActor *a)
{
    s16 *cx;
    s16 *cz;
    s32 x, z, delta;

    delta = s->targetX - s->currentX;
    cx = &s->currentX;
    x = Spider_QuarterDisplacement(delta);
    delta = s->targetZ - s->currentZ;
    cz = &s->currentZ;
    delta = Spider_QuarterDisplacement(delta);
    z = delta;
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    a->base.body.motion.x += x;
    a->base.body.motion.z += z;
    *cx += x;
    *cz += z;
    if (!x && !z)
        return 1;
    return 0;
}

struct LandingReady {
    u8 value;
};
static inline u32 Spider_SetLandingReady(struct LandingReady *p)
{
    u32 zero = 0;
    p->value = 1;
    return zero;
}

void Spider_UpdateLandingMotion(struct ChaseActor *a, s32 frame)
{
    struct LandingSequence *seq = (void *)(((u8 *)a->base.drawable) + 0x48);
    u32 *flags;
    if (Spider_TakeEntry(&a->base.field2B2)) {
        Spider_ClearContextFlags(&((struct SpiderRecoveryContext *)a->base.context)->flags, ~6);
        SceneActorAnimation_SelectDirectionalEntry(a, 3);
        Spider_AddMotionFlags(a, 0x20);
    }
    {
        u32 mask = 0x10000;
        u32 *p = &a->base.flags2D0;
        *p |= mask;
    }
    {
        u32 mask = 0x2000;
        u32 *p = &a->base.flags2CC;
        *p |= mask;
        flags = p;
    }
    if (frame <= 17 && seq->frame == 1)
        seq->elapsed = 0;
    if (Spider_MoveLanding((void *)a, a)) {
        struct SceneActorRegistryControlGroup *group = &a->group.common;
        u32 zero = Spider_SetLandingReady((void *)&a->base.field2B0);
        Spider_SetState(a, zero, zero, 12);
        Spider_SetControl(a, group->gate.control, 26);
        sub_080A4798(a);
        SceneActorMovement_ResolveRoutePosition(a);
        Spider_AddGroupFlags(&a->group.common, 4);
        Spider_AddMotionFlags(a, 0x80);
        a->group.common.gate.counter = a->group.common.gate.duration;
        Spider_ClearMotionFlags(a, ~0x20);
        a->base.body.direction = a->base.tracking.heading;
        {
            u32 mask = 0x10;
            *flags |= mask;
        }
    }
}
