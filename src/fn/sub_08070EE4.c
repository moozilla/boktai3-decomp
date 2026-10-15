#include "scene_actor_registry.h"
#define SceneActorRegistry_TestScriptActorLiving sub_08070EE4
struct SceneActorRegistryActor *sub_08070EBC(u32);
s32 SceneActorRegistry_TestScriptActorLiving(void)
{
    struct SceneActorRegistryActor *actor;
    s32 result;
    s32 health;
    u32 id = Script_GetValue();
    if (!id || (actor = sub_08070EBC(id)) == NULL)
        return 0;
    health = actor->health;
    result = 1;
    if (health <= 0)
        result = 0;
    return result;
}
