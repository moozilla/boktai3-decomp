#include "global.h"
#include "scene_actor_reset.h"
s32 sub_0812E7EC(struct SceneActorRegistryResetView *);
void SceneActor_DispatchActivePhase(struct SceneActorRegistryResetView *actor)
{
    if ((u8)sub_0812E7EC(actor))
        Callback_InvokeActor(actor, actor->phaseCallbacks[actor->phase2A9]);
}
