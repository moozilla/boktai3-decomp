#include "scene_actor_registry.h"
#define SceneActorRegistry_GetFirstActor sub_08070EB0
struct SceneActorRegistryActor *SceneActorRegistry_GetFirstActor(void)
{
    return gUnk_020004AC->actor;
}
