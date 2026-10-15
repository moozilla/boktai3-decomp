#include "scene_actor_script.h"
#define SceneActorScript_ClearManagerFlagsFromScript sub_080727EC
void SceneActorScript_ClearManagerFlagsFromScript(void)
{
    struct SceneActorRegistryManager *manager = gUnk_020004A8;
    if (manager)
        manager->flags &= ~(u16)Script_GetKeywordValue('s', 0);
}
