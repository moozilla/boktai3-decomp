// MATCHING_CONCESSION: register-pinning
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
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *);
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
                              u32 zero)
{
    u8 code = 0x2B;
    actor->field2B2 = 1;
    actor->field2B0 = zero8;
    actor->field2B5 = code;
    actor->target = fn;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
void SceneActorApproach_RunAlternateTimedAction(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorApproachActionContext *context = actor->context;
    if (TakeReady(actor)) {
        register u8 zero8 asm("r3");
        register u32 zero asm("r5");
        u32 flags = 0x100;
        u32 off = 0x2D4;
        register u16 *flagptr asm("r2");
        u32 value;
        flagptr = (u16 *)((u8 *)actor + off);
        value = *flagptr;
        zero8 = 0;
        zero = 0;
        asm("" : "+r"(zero8), "+r"(zero));
        flags |= value;
        *flagptr = flags;
        SetControl(actor, SceneActorReaction_TickMultiHitAnimation, zero8, zero);
        context->timer = context->duration;
        Vorn_SpawnHazard(actor, 3);
    }
    if (actor->field2B0) {
        ClearFlags(&actor->flags2D4, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->counter2AC++;
}
