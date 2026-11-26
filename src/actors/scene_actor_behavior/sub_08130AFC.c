#include "engine/shared/global.h"
#include "engine/shared/scene_actor_terrain_reaction.h"
s32 Callback_InvokeActor(void *, void *);

void SceneActorState_DispatchTable2A0State(struct ChaseActor *actor)
{
    Callback_InvokeActor(actor, actor->base.callbacks2A0[actor->base.phase2A9]);
}
