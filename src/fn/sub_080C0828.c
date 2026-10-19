
#ifdef NONMATCHING
// fuzzy: 90.5%
// foundry draft (worker best 90.5%); what differs is mostly register allocation and instruction order
#define SceneActorApproach_RunAlternateTimedAction sub_080C0828
#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorApproachActionContext {
    u8 unknown0[0x6F4];
    u16 timer;
    u8 unknown6F6[6];
    u16 duration;
};
void SceneActorReaction_TickMultiHitAnimation(void);
void Vorn_SpawnHazard(struct SceneActorRegistryResetView *, s32);
void sub_080B7C10(struct SceneActorRegistryResetView *);
static inline u8 TakeReady(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void ClearFlags(u16 *p, u32 mask)
{
    *p &= mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *fn, u8 zero8,
                              s16 zero)
{
    u8 code = 0x2B;
    {
        u8 *taken = &actor->field2B2;
        *taken = 1;
    }
    do {
        u32 field = 0x2B0;
        do {
            ((u8 *)actor)[field] = zero8;
        } while (0);
        field += 5;
        ((u8 *)actor)[field] = code;
        actor->target = fn;
        field += 0x13;
        *(u16 *)((u8 *)actor + field) = zero;
    } while (0);
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
void sub_080C0828(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorApproachActionContext *context = actor->context;
    if (TakeReady(actor)) {
        u32 flags = 0x100;
        u16 *flagptr = &actor->flags2D4;
        u32 value = *flagptr;
        u8 zero8 = 0;
        u32 zero = 0;
        flags |= value;
        *flagptr = flags;
        SetControl(actor, SceneActorReaction_TickMultiHitAnimation, zero8, zero);
        context->timer = context->duration;
        Vorn_SpawnHazard(actor, 3);
    }
    if (actor->field2B0) {
        ClearFlags(&actor->flags2D4, ~0x100);
        sub_080B7C10(actor);
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C0828);
#endif
