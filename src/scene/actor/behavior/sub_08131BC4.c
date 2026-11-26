#include "engine/engine_pause.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_behavior.h"
#include "global.h"
extern u32 gUnk_02000560;

void SceneActorBehavior_DispatchOrEnterState2(struct ChaseActor *actor)
{
    if (Engine_PauseBits(0x400) ||
        SceneActorBehavior_OverrideBits(4, &gUnk_02000560)) {
        SceneActorTerrain_EnterState(actor, 2, 0, 1);
    } else {
        Callback_InvokeActor(actor, actor->base.behaviorCallbacks[actor->base.behavior2AA]);
    }
}
