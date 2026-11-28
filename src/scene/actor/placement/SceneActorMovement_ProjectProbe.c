#include "scene/actor/scene_actor_state.h"
#include "scene/actor/scene_actor_registry.h"
void SceneActorMovement_ProjectProbe(struct ChaseActor *actor, u32 xScale, u32 zScale)
{
    xScale <<= 2;
    zScale <<= 2;
    actor->base.reactionStart.x =
        actor->base.body.position.x +
        ((s32)(gUnk_086149C4[(actor->base.reactionDirection + 64) & 255] * xScale) >> 12);
    actor->base.reactionStart.y = actor->base.body.position.y;
    actor->base.reactionStart.z =
        actor->base.body.position.z +
        ((s32)(gUnk_086149C4[actor->base.reactionDirection] * zScale) >> 12);
    actor->base.reactionEnd = actor->base.body.position;
}
