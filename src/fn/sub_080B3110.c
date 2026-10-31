#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
void sub_080B1FA8(void *);
void sub_080ADB90(void);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
#define Vorn_BeginResponsePursuit sub_080B3110
struct VornResponseFlags {
    s16 bits : 16;
};
static inline u32 Vorn_ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline u8 Vorn_TestContext(struct SceneActorRegistryResetView *context, u16 mask,
                                  struct VornResponseFlags **field)
{
    struct VornResponseFlags *flags = (void *)&context->flags6D4;
    u16 value = flags->bits;
    value &= mask;
    *field = flags;
    if (value)
        return 1;
    return 0;
}
static inline u8 Vorn_TestResponseFlags(struct VornResponseFlags *flags, u32 mask)
{
    u16 value = flags->bits;
    if (value & mask)
        return 1;
    return 0;
}
static inline void Vorn_SetResponseFlags(struct VornResponseFlags *flags, u16 bits)
{
    flags->bits |= bits;
}
static inline void Vorn_SetTimedMotion(struct SceneActorRegistryResetView *actor, void *callback,
                                       u32 duration)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = duration;
    actor->flags114 = 1;
}
static inline void Vorn_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags = bits | tracking->flags;
}
static inline void Vorn_ResetContext(struct SceneActorRegistryResetView *a)
{
    struct SceneActorChaseTracking *motion;
    struct SceneActorRegistryControl *control;
    u16 *flags;
    a->field2C2 = 64;
    a->group.common.gate.flags = 0;
    a->group.common.gate.counter = 0;
    flags = &a->flags2D4;
    {
        s32 mask = ~0x380;
        *flags = mask & (*flags);
    }
    motion = &a->tracking;
    {
        s32 mask = ~0x18;
        motion->flags = mask & motion->flags;
    }
    control = &a->group.common.control;
    if (Vorn_ActorFlags(a, 0x200000)) {
        u32 *state = &a->flags2D0;
        s32 mask = ~0x1000000;
        *state = mask & (*state);
        {
            s32 mask = ~0x200;
            *flags = mask & (*flags);
        }
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
    {
        s32 mask = ~0x400;
        *flags = mask & (*flags);
    }
    a->group.turn.turnsRemaining = 255;
}

void Vorn_BeginResponsePursuit(struct SceneActorRegistryResetView *a, u32 reset, const void *unused,
                               u32 unused2)
{
    struct SceneActorRegistryResetView *ctx = (void *)a->context;
    struct SceneActorChaseGroup *group = &ctx->group;
    struct VornResponseFlags *flags;

    if (ctx->field69E)
        return;
    if (Vorn_TestContext(ctx, 1, &flags) && !Vorn_TestResponseFlags(flags, 2))
        return;
    ctx->field69E = 60;
    Vorn_ResetContext(ctx);
    Vorn_SetResponseFlags(flags, 1);
    flags->bits &= ~2;
    ctx->field698 = ctx->frame69A;
    a->body.direction = a->field2B6;
    if (reset == 1)
        SceneBody_StartRoute(&a->body.id, &group->homeRoute,
                             group->homeRoute.branches[1 ^ group->homeRoute.activeBranch], 0, 0);
    sub_080B1FA8(a);
    {
        u32 mask = 4;
        a->flags2D0 = mask | a->flags2D0;
    }
    {
        s8 mask = ~16;
        a->flags2CC = mask & a->flags2CC;
    }
    SceneActorChase_SetBehavior(a, 15);
    SceneActorChase_SetControl(a, a->savedTarget, 0);
    Vorn_SetTimedMotion(a, sub_080ADB90, 35);
    Vorn_SetTrackingFlags(&a->tracking, 8);
    {
        u32 mask = 128;
        a->flags2CC = mask | a->flags2CC;
    }
}
