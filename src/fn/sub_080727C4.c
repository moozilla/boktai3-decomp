#include "scene_actor_script.h"
#define SceneActorScript_SetManagerFlagsFromScript sub_080727C4
void SceneActorScript_SetManagerFlagsFromScript(void)
{
    struct SceneActorRegistryManager *manager = gUnk_020004A8;
    if (manager)
        manager->flags |= (u16)Script_GetKeywordValue('s', 0);
}
