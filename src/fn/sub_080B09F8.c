#include "global.h"
#include "scene_actor_control_phase.h"
#ifdef NONMATCHING
// fuzzy: 98.2%
// foundry draft (worker best 98.2%); what differs is mostly register allocation and instruction order
#define SceneActorPhase_UpdateCooldown sub_080B09F8
void SceneActor_UpdateTargetedAttack(struct SceneActorRegistryResetView *);
void sub_080B2858(struct SceneActorRegistryResetView *, u32);
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
void sub_080B09F8(struct SceneActorRegistryResetView *p)
{
    struct SceneActorAnimationLink *link = p->context;
    if (SceneActorControl_TakeFlag(&p->field2B1)) {
        u16 shortZero = PrepareFlags(p);
        u32 zero = 0;
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
        sub_080B2858(p, 1);
    } else ++p->counter2AC;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B09F8);
#endif
