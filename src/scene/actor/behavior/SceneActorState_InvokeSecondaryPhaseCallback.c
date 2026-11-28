#include "global.h"
#include "scene/motion/scene_actor_terrain_reaction.h"

s32 Callback_InvokeActor(void *, void *);

void SceneActorState_InvokeSecondaryPhaseCallback(struct ChaseActor *actor)
{
    Callback_InvokeActor(actor, actor->base.callbacks290[actor->base.phase2A9]);
}
