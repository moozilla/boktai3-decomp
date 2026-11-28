#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/motion/scene_actor_terrain_reaction.h"

void CentipedeBody_UpdateSpreadWindup(void);
void Centipede_SelectWanderRoute(struct ChaseActor *);
void SceneActorPlacement_UpdateFlaggedResponse(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        SceneActorChase_SetControl(p, CentipedeBody_UpdateSpreadWindup, 38);
        MonsterInit_SetStatusFlags(p, 0x100000);
        Sound_PlaySong(0x19B);
    }
    if (p->base.controlKind == 39 && (u8)Callback_InvokeActor(p, p->base.contactQuery.opaque)) {
        SceneActorTerrain_EnterState(p, 0, 2, 0);
    } else if (p->base.stepDone) {
        Centipede_SelectWanderRoute(p);
        SceneActorTerrain_EnterState(p, 0, 0, 2);
        {
            u32 *flags = &p->base.statusFlags;
            u32 mask = ~0x100000;
            *flags &= mask;
        }
    } else {
        p->base.counter2AC++;
    }
}
