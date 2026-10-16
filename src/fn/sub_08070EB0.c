#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_GetFirstActor(void)
{
    return gUnk_020004AC->actor;
}
