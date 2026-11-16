#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"
void SceneActor_UpdateTargetedAttack(struct ChaseActor *);
void Vorn_SelectRetreatRoute(struct ChaseActor *, u32);
void SceneActorPhase_UpdateCooldown(struct ChaseActor *p)
{
    struct SceneActorAnimationLink *link = p->base.context;
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        {
            s32 mask = 0x100;
            struct ChaseFlags40 *flags = (void *)&p->base.flags2D4;
            flags->bits |= mask;
        }
        Chase_SetCtl(p, SceneActor_UpdateTargetedAttack, 0x22);
        link->phaseDuration = 120;
        {
            u32 mask = 4;
            p->base.flags2D0 |= mask;
        }
    }
    if (p->base.field2B0) {
        u16 *flags = &p->base.flags2D4;
        u32 mask = ~0x100;
        *flags = mask & *flags;
        SceneActorAnimation_SelectDirectionalEntry((void *)p, 0);
        Vorn_SelectRetreatRoute(p, 1);
    } else
        ++p->base.counter2AC;
}
