#include "scene_actor_script.h"
#define SceneActorScript_SetManagerModeFromScript sub_08072830
void SceneActorScript_SetManagerModeFromScript(void)
{
    struct SceneActorRegistryManager *manager = gUnk_020004A8;
    if (manager)
        manager->mode = Script_GetKeywordValue('l', 3);
}
