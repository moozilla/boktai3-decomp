#include "scene/actor/scene_actor_registry.h"
struct ChaseActor *SceneActorRegistry_GetFirstActor(void)
{
    return gSceneActorRegistryNode->actor;
}
