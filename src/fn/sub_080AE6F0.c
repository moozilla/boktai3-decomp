// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 83.1%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: resumes the route or runs the recovery script.
// Remaining differences: saved-zero lifetimes, context flag scheduling, and register allocation; caching flags reduces register differences.
extern const u8 gUnk_0824E20C[];
u8 Vorn_HasActiveResponse(void *);
u8 SceneActor_DispatchContextAction(void *, u32);
void sub_080B3110(void *, u32, const u8 *, u32);
void Vorn_SendMessage(void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
static inline void StartRoute(struct SceneActorRegistryResetView *a, u8 *group)
{
    SceneBody_StartRoute(&a->id, group + 0x74, group[0x84], group[0x85], group[0x86]);
}
static inline void ClearMotion(u16 *flags, s32 mask)
{
    *flags &= mask;
}
void Vorn_ResumeReactionRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group);
    u8 *context = actor->context;
    register u32 *flags asm("r8");
    u16 *motionFlags;
    u16 *contextFlags;
    register u32 zero asm("sl");
    typeof(flags) c1;
    u32 k1;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->direction = (((actor->direction + 128) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    }
    flags = &actor->flags2D0;
    c1 = flags;
    {
        u32 value = *c1;
        if (!(value & 16))
            return;
        value &= ~4;
        *c1 = value;
    }
    Vorn_SendMessage(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    motionFlags = &actor->flags2D4;
    {
        s32 mask = ~0x38;
        *motionFlags &= mask;
    }
    zero = 0;
    asm volatile("" : "+r"(zero));
    StartRoute(actor, group);
    contextFlags = (u16 *)(context + 0x6D4);
    {
        s32 mask = ~(k1 = 10);
        *contextFlags &= mask;
    }
    if (Vorn_HasActiveResponse(context)) {
        struct SceneActorReactionMotion *motion = (void *)(&actor->fieldDC);
        u32 mask = 8;
        u8 value = motion->flags;
        value |= mask;
        motion->flags = value;
        mask = 0x80;
        actor->flags2CC |= mask;
        {
            u16 *f = (u16 *)(((u8 *)actor->context) + 0x6D4);
            s32 mask = ~3;
            *f &= mask;
        }
        sub_080B3110(actor, 1, gUnk_0824E20C, 0x39F);
    } else if (!SceneActor_DispatchContextAction(context, 1)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        u32 *actionFlags;
        {
            s32 mask = ~0x1E;
            *contextFlags &= mask;
        }
        do {
            actor->field2C2 = 64;
        } while (0);
        *((u16 *)(((u8 *)actor) + 0x414)) = 0;
        *((u16 *)(((u8 *)actor) + 0x418)) = 0;
        ClearMotion(motionFlags, ~0x380);
        motion = (void *)(&actor->fieldDC);
        {
            s32 mask = ~0x18;
            motion->flags &= mask;
        }
        control = &actor->group.control;
        actionFlags = &actor->flags2CC;
        if ((*actionFlags) & 0x200000) {
            *flags &= ~0x1000000;
            ClearMotion(motionFlags, ~0x200);
            *(((u8 *)control) + k1) = zero;
            control->counter = 0;
            control->timer = 0;
        }
        ClearMotion(motionFlags, ~0x400);
        group[0x206] = 0xFF;
        *actionFlags |= 16;
        Chase_SetState(actor, 3, 1, 0);
        Chase_SetCtl(actor, *((void **)(group + 0x210)), 15);
    }
    actor->field2C2 = 64;
}

#else
INCLUDE_ASM("asm/nonmatching", Vorn_ResumeReactionRoute);
#endif
