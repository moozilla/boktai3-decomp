#include "scene_actor_registry.h"
#define SceneActorRegistry_TestScriptActorPresent sub_08070F10
struct SceneActorRegistryActor *sub_08070EBC(u32);
u32 SceneActorRegistry_TestScriptActorPresent(void)
{
    u32 id = Script_GetValue();
    if (id && sub_08070EBC(id))
        return 1;
    return 0;
}
