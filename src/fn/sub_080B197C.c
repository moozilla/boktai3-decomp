#include "scene_actor_chase.h"

extern const u8 gUnk_0824E230[];
void sub_080B3110(void *, u32, const void *, u32);
u8 SceneActor_DispatchContextAction(void *, u32);
static inline u8 HasFlag(u32 value, u32 mask)
{
    mask &= value;
    if (mask)
        return 1;
    return 0;
}
// The u16 parameter here and the u8 mask of MotionFlag lengthen the actor pointer's life by a
// few insns before combine, enough that `active` (8 refs) outranks it for r4.
static inline u8 ShouldReact(struct SceneActorRegistryResetView *ctx, u16 value)
{
    if (HasFlag(value, 1))
        return 1;
    if (ctx->field67C >= 0)
        return 1;
    return 0;
}
static inline u8 MotionFlag(struct SceneActorReactionMotion *m, u8 mask)
{
    if (m->flags & mask)
        return 1;
    return 0;
}
static inline void SetMotion(struct SceneActorReactionMotion *m, u8 mask)
{
    m->flags = mask | m->flags;
}

// confidence=low: selects a response script or idle control from context state.
#define Vorn_SelectReactionControl sub_080B197C
static inline void MarkReactionMotion(struct SceneActorReactionMotion *m)
{
    u8 mask = 8;
    m->flags = mask | m->flags;
}

void Vorn_SelectReactionControl(struct SceneActorRegistryResetView *a)
{
    struct SceneActorRegistryResetView *ctx = a->context;
    u16 *flags = &ctx->flags6D4;
    u32 value = ~8;
    value &= *flags;
    *flags = value;
    if (ShouldReact(ctx, value)) {
        sub_080B3110(a, 1, gUnk_0824E230, 0x279);
        MarkReactionMotion((void *)&a->tracking.target);
        {
            u32 mask = 0x80;
            a->flags2CC = mask | a->flags2CC;
        }
    } else {
        u32 active = MotionFlag((void *)&a->tracking.target, 8);
        if (active)
            SceneActor_DispatchContextAction(ctx, 0);
        else {
            Chase_SetState(a, active, 1, active);
            Chase_SetCtl(a, SceneActorControl_OnIdle, 16);
        }
        SetMotion((void *)&a->tracking.target, 8);
    }
}
