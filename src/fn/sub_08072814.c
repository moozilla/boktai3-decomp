#include "scene_actor_script.h"
u16 SceneActorScript_GetManagerField62(void)
{
    u16 value;
    if (gSceneActorRegistryManager)
        value = gSceneActorRegistryManager->field62;
    else
        value = 0;
    return value;
}
