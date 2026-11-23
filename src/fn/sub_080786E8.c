#include "scene_actor_registry.h"
void Shadow_Initialize(void *, void *, u32);
void SceneActorBehavior_InitializeShadow(struct ChaseActor *actor)
{
    Shadow_Initialize(actor->base.shadow, &actor->base.body.position, 0);
}
