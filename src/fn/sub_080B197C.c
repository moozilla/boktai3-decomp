// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: motion-pointer lifetime and state reset scheduling.
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
static inline void SetMotion_1(struct SceneActorReactionMotion *m)
{
    {
        u8 k1 = 8;
        m->flags = k1 | m->flags;
    }
}

void Vorn_SelectReactionControl(struct SceneActorRegistryResetView *a)
{
    u8 *ctx = a->context;
    u16 *flags = (void *)(ctx + 0x6D4);
    register s32 mask asm("r1") = ~8;
    register u32 value asm("r1") = mask & (*flags);
    asm("" : "+r"(value));
    *flags = value;
    if (ShouldReact(ctx, value)) {
        sub_080B3110(a, 1, gUnk_0824E230, 0x279);
        SetMotion_1((void *)(&a->fieldDC));
        {
            u32 mask = 0x80;
            a->flags2CC = mask | a->flags2CC;
        }
    } else {
        struct SceneActorReactionMotion *m = (void *)(&a->fieldDC);
        register u32 active asm("r4") = MotionFlag(m, 8);
        asm("" : "+r"(active));
        if (active)
            SceneActor_DispatchContextAction(ctx, 0);
        else {
            a->phase2A9 = 1;
            {
                typeof(a->state2A8) *q1 = &a->state2A8;
                do {
                    *q1 = active;
                } while (0);
            }
            a->behavior2AA = active;
            a->counter2AC = active;
            a->field2B1 = 1;
            Chase_SetCtl(a, SceneActorControl_OnIdle, 16);
        }
        SetMotion(m, 8);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080B197C);
#endif
