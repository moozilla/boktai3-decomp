#include "engine/shared/debug.h"
#include "actors/scene_actor_reaction/scene_actor_reaction.h"
#include "actors/scene_actor_animation_and_scripted_actions/scene_actor_animation.h"
void SceneActorReaction_UpdateAnimationGate(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        DEBUG_PRINT();
    }
    if (actor->base.tracking.distance <= 0xFFFFF) {
        u16 *field = &group->gate.flags;
        u32 mask = 1;
        mask |= *field;
        *field = mask;
    } else {
        u16 *field = &group->gate.flags;
        u32 mask = ~1;
        mask &= *field;
        *field = mask;
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
}
