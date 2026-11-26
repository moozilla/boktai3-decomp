#include "global.h"
#include "scene/actor/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_behavior.h"
extern u32 gUnk_02000560;

void SceneActorBehavior_DispatchPrimaryOrEnterState2(struct ChaseActor *actor)
{
    if (SceneActorBehavior_GlobalPauseBits(0x400) ||
        SceneActorBehavior_OverrideBits(4, &gUnk_02000560)) {
        SceneActorTerrain_EnterState(actor, 2, 0, 1);
    } else {
        Callback_InvokeActor(actor, actor->base.callbacks284[actor->base.behavior2AA]);
    }
}
