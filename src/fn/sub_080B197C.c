// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"

extern const u8 gUnk_0824E230[];
void sub_080B3110(void *, u32, const void *, u32);
u8 SceneActor_DispatchContextAction(void *, u32);
static inline u8 HasFlag(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 ShouldReact(u8 *ctx, u32 value)
{
    if (HasFlag(value, 1) || *(s16 *)(ctx + 0x67C) >= 0)
        return 1;
    return 0;
}
static inline u8 MotionFlag(struct SceneActorReactionMotion *m, u32 mask)
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
    u8 *ctx = a->context;
    u16 *flags = (void *)(ctx + 0x6D4);
    register u32 value asm("r1") = ~8;
    value &= *flags;
    *flags = value;
    if (ShouldReact(ctx, value)) {
        sub_080B3110(a, 1, gUnk_0824E230, 0x279);
        MarkReactionMotion((void *)&a->fieldDC);
        {
            u32 mask = 0x80;
            a->flags2CC = mask | a->flags2CC;
        }
    } else {
        u32 active = MotionFlag((void *)&a->fieldDC, 8);
        /* Keep the motion test result available before selecting its control. */
        asm("" : : "r"(active));
        if (active)
            SceneActor_DispatchContextAction(ctx, 0);
        else {
            Chase_SetState(a, active, 1, active);
            Chase_SetCtl(a, SceneActorControl_OnIdle, 16);
        }
        SetMotion((void *)&a->fieldDC, 8);
    }
}
