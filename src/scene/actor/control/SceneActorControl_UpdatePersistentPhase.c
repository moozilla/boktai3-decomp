#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/actor/scene_actor_reaction.h"
void SceneActorControl_UpdatePersistentAnimation(struct ChaseActor *);
void SceneActorControl_UpdatePersistentPhase(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, (void *)SceneActorControl_UpdatePersistentAnimation,
                                         34);
        ActorShadow_Disable(actor->base.shadow);
    }
    if (!((u8(*)(struct ChaseActor *))actor->base.contactQuery.opaque)(actor))
        ++actor->base.counter2AC;
}
