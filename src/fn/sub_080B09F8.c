#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"
void SceneActor_UpdateTargetedAttack(struct SceneActorRegistryResetView *);
void Vorn_SelectRetreatRoute(struct SceneActorRegistryResetView *, u32);
void SceneActorPhase_UpdateCooldown(struct SceneActorRegistryResetView *p)
{
    struct SceneActorAnimationLink *link = p->context;
    if (SceneActorControl_TakeFlag(&p->field2B1)) {
        {
            s32 mask = 0x100;
            struct ChaseFlags40 *flags = (void *)&p->flags2D4;
            flags->bits |= mask;
        }
        Chase_SetCtl(p, SceneActor_UpdateTargetedAttack, 0x22);
        link->phaseDuration = 120;
        {
            u32 mask = 4;
            p->flags2D0 |= mask;
        }
    }
    if (p->field2B0) {
        u16 *flags = &p->flags2D4;
        u32 mask = ~0x100;
        *flags = mask & *flags;
        SceneActorAnimation_SelectDirectionalEntry((void *)p, 0);
        Vorn_SelectRetreatRoute(p, 1);
    } else
        ++p->counter2AC;
}
