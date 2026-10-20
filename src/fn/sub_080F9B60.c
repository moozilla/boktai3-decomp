#include "scene_actor_chase.h"

void Axe_ReleaseAuxiliarySprite(void *);
void SceneActorControl_OnIdle(void);

#define Axe_ReleaseAndIdle sub_080F9B60
void Axe_ReleaseAndIdle(struct SceneActorRegistryResetView *actor)
{
    Axe_ReleaseAuxiliarySprite(actor);
    Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
    Chase_SetBehavior(actor, 4);
}
