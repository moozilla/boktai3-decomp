// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
// foundry draft (worker best 98.2%); what differs is mostly register allocation and instruction order
void SceneActor_UpdateTargetedAttack(struct SceneActorRegistryResetView *);
void Vorn_SelectRetreatRoute(struct SceneActorRegistryResetView *, u32);
static inline u8 PhaseComplete(struct SceneActorRegistryResetView *p)
{
    return p->field2B0;
}
static inline u16 PrepareFlags(struct SceneActorRegistryResetView *p)
{
    u32 mask = 0x100;
    u16 *flags = &p->flags2D4;
    *flags = mask | *flags;
    return 0;
}
void SceneActorPhase_UpdateCooldown(struct SceneActorRegistryResetView *p)
{
    struct SceneActorAnimationLink *link = p->context;
    if (SceneActorControl_TakeFlag(&p->field2B1)) {
        u32 mask0 = 0x100;
        u16 *flags0 = &p->flags2D4;
        register u16 old asm("r1") = *flags0;
        u16 shortZero = 0;
        u32 zero = 0;
        asm("" : "+r"(shortZero));
        *flags0 = mask0 | old;
        {
            void *control = SceneActor_UpdateTargetedAttack;
            u32 kind = 0x22;
            p->field2B2 = 1;
            p->field2B0 = zero;
            p->field2B5 = kind;
            p->target = control;
            p->field2C8 = shortZero;
            {
                u32 *flags = &p->flags114;
                s32 mask = -2;
                *flags &= mask;
            }
            p->word104 = shortZero;
        }
        link->phaseDuration = 120;
        {
            u32 mask = 4;
            p->flags2D0 |= mask;
        }
    }
    if (PhaseComplete(p)) {
        u16 *flags = &p->flags2D4;
        u32 mask = ~0x100;
        *flags = mask & *flags;
        SceneActorAnimation_SelectDirectionalEntry((void *)p, 0);
        Vorn_SelectRetreatRoute(p, 1);
    } else
        ++p->counter2AC;
}
