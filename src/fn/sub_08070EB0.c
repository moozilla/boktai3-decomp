#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_GetFirstActor(void)
{
    return gSceneActorRegistryNode->actor;
}
