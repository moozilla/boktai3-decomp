#include "global.h"
#include "scene_actor_reset.h"
s32 SceneActorReaction_DispatchPendingCommands(struct SceneActorRegistryResetView *);
void SceneActor_DispatchActivePhase(struct SceneActorRegistryResetView *actor)
{
    if ((u8)SceneActorReaction_DispatchPendingCommands(actor))
        Callback_InvokeActor(actor, actor->phaseCallbacks[actor->phase2A9]);
}
