#include "scene_actor_script.h"
void SceneActorScript_SetManagerModeFromScript(void)
{
    struct SceneActorRegistryManager *manager = gSceneActorRegistryManager;
    if (manager)
        manager->mode = Script_GetKeywordValue('l', 3);
}
