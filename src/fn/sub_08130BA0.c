#define SceneActor_DispatchActivePhase sub_08130BA0
#define Callback_InvokeActor           sub_0824923C
#include "global.h"
#include "scene_actor_reset.h"
s32 sub_0812E7EC(struct SceneActorRegistryResetView *);
void sub_08130BA0(struct SceneActorRegistryResetView *actor)
{
    if ((u8)sub_0812E7EC(actor))
        Callback_InvokeActor(actor, actor->phaseCallbacks[actor->phase2A9]);
}
