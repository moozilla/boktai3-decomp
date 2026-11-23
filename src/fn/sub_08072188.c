#include "scene_actor_palette.h"
#include "scene_actor_control_helpers.h"
#include "word_flags.h"
void SceneActorPalette_ClearCompletedTransitions(void)
{
    struct SceneActorPaletteManagerView *manager = (void *)gSceneActorRegistryManager;
    s32 pending;
    if (!manager || !WordFlags_Has(0x2000, &manager->flags))
        return;
    if (SceneActorControl_TestFlags(gEngineFlags, 0x4800)) {
        pending = SceneActorRegistry_CountLivingByCategory(10);
    } else {
        if ((u8)SceneActorRegistry_TestCategoryFlags(10, 0x20000))
            return;
        pending = SceneActorRegistry_TestCategoryFlags(23, 0x20000) << 24;
    }
    if (!pending)
        WordFlags_And(~0x22000, &manager->flags);
}
