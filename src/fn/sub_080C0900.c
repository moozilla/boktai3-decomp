// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"

// foundry draft (worker best 94.9%); what differs is mostly register allocation and instruction order
struct SceneActorApproachActionContext {
    u8 unknown0[0x6F2];
    u16 timer;
    u8 unknown6F4[6];
    u16 duration;
    u8 unknown6FC[0x14];
    struct ContactPosition destination;
};
void sub_080BAAF4(void);
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
    u8 code = 0x2C;
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
void SceneActorApproach_CaptureTargetAndRunAction(struct SceneActorRegistryResetView *actor)
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
        SetControl(actor, sub_080BAAF4, zero8, zero);
        context->destination = actor->tracking.target->position;
        context->timer = context->duration;
        Vorn_SpawnHazard(actor, 2);
    }
    if (actor->field2B0) {
        ClearFlags(&actor->flags2D4, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->counter2AC++;
}
