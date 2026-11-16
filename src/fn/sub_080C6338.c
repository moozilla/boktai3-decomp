#include "scene_actor_chase.h"
void Golem_UpdateChargeBuildup(void);

void Golem_BeginChargeBuildup(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 0xE);
        {
            u32 mask = 0x100;
            actor->base.motionFlags = mask | actor->base.motionFlags;
        }
    }
    if (actor->base.restart)
        Chase_SetCtl(actor, Golem_UpdateChargeBuildup, 0x22);
}
