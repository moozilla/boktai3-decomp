#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32);
s32 SceneActorRegistry_TestScriptActorLiving(void)
{
    struct SceneActorRegistryActor *actor;
    s32 result;
    s32 health;
    u32 id = Script_GetValue();
    if (!id || (actor = SceneActorRegistry_FindById(id)) == NULL)
        return 0;
    health = actor->health;
    result = 1;
    if (health <= 0)
        result = 0;
    return result;
}
