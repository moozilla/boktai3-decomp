#include "engine/shared/global.h"
#include "engine/shared/scene_actor_reset.h"
s32 SceneActorReaction_DispatchPendingCommands(struct ChaseActor *);
void SceneActor_DispatchActivePhase(struct ChaseActor *actor)
{
    if ((u8)SceneActorReaction_DispatchPendingCommands(actor))
        Callback_InvokeActor(actor, actor->base.phaseCallbacks[actor->base.phase2A9]);
}
