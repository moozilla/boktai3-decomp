// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"

struct SceneActorApproachActionContext {
    u8 unknown0[0x6F0];
    u16 timer;
    u8 unknown6F2[6];
    u16 duration;
};
void SceneActorReaction_TickQueuedContact(void);
void SceneActorReaction_TickContactAnimation(void);
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
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *fn, u32 zero,
                              u8 zero8)
{
    u8 code = 3;
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
        {
            u16 *q = (u16 *)((u8 *)actor + field);
            asm("" : "+r"(q), "+r"(field));
            *q = zero;
        }
    } while (0);
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
void SceneActorApproach_RunCategoryAction(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorApproachActionContext *context = actor->context;
    if (TakeReady(actor)) {
        u32 flags = 0x100;
        u16 *flagptr = &actor->flags2D4;
        u32 value = *flagptr;
        u8 zero8 = 0;
        u32 zero = 0;
        asm("" : "+r"(zero));
        flags |= value;
        *flagptr = flags;
        if (actor->category == 7)
            SetControl(actor, SceneActorReaction_TickQueuedContact, zero, zero8);
        else
            SetControl(actor, SceneActorReaction_TickContactAnimation, zero, zero8);
        Vorn_SpawnHazard(actor, 1);
        context->timer = context->duration;
    }
    if (actor->field2B0) {
        ClearFlags(&actor->flags2D4, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->counter2AC++;
}
