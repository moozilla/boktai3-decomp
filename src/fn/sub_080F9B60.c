#include "scene_actor_chase.h"

void Axe_ReleaseAuxiliarySprite(void *);

void Axe_ReleaseAndIdle(struct ChaseActor *actor)
{
    Axe_ReleaseAuxiliarySprite(actor);
    Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
    Chase_SetBehavior(actor, 4);
}
