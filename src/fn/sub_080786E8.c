#include "scene_actor_registry.h"
void Shadow_Initialize(void *, void *, u32);
void SceneActorBehavior_InitializeShadow(struct ChaseActor *actor)
{
    Shadow_Initialize((u8 *)actor + 0x320, &actor->base.body.position, 0);
}
