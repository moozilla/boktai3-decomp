#include "scene/actor/scene_actor_palette.h"
#include "scene/actor/scene_actor_control_helpers.h"
#include "engine/word_flags.h"
void SceneActorPalette_ClearCompletedTransitions(void)
{
    struct SceneActorRegistryManager *manager = gSceneActorRegistryManager;
    if (!manager || !WordFlags_Has(0x2000, &manager->flags))
        return;
    if (SceneActorControl_TestFlags(gEngineFlags, 0x4800)) {
        if (SceneActorRegistry_CountLivingByCategory(10))
            return;
    } else {
        if ((u8)SceneActorRegistry_TestCategoryFlags(10, 0x20000))
            return;
        if ((u8)SceneActorRegistry_TestCategoryFlags(23, 0x20000))
            return;
    }
    WordFlags_And(~0x22000, &manager->flags);
}
