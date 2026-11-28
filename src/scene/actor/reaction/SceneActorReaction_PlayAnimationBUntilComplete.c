#include "scene/actor/scene_actor_reaction.h"
#include "npc/manager/scene_actor_animation.h"

void SceneActorReaction_PlayAnimationBUntilComplete(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0xB);
    if (actor->base.restart)
        actor->base.stepDone = 1;
}
