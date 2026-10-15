#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32);
u32 SceneActorRegistry_TestScriptActorPresent(void)
{
    u32 id = Script_GetValue();
    if (id && SceneActorRegistry_FindById(id))
        return 1;
    return 0;
}
