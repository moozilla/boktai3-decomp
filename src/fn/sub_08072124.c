#include "scene_actor_palette.h"
#include "scene_actor_control_helpers.h"
#include "word_flags.h"
/* SceneActorPalette_IsTransitionComplete: Reports completion when transitions are disabled or no relevant living actors remain; does not inspect frame counters */
#define SceneActorPalette_IsTransitionComplete sub_08072124
#define SceneActorPalette_IsTransitionComplete sub_08072124

u32 SceneActorPalette_IsTransitionComplete(void)
{
    u32 id;
    struct SceneActorPaletteManagerView *manager = (void *)gSceneActorRegistryManager;
    if (manager == NULL || !WordFlags_Has(0x2000, &manager->flags))
        return 1;
    if (SceneActorControl_TestFlags(gEngineFlags, 0x4800))
        id = 10;
    else {
        if (SceneActorRegistry_CountLivingByCategory(10))
            return 0;
        id = 23;
    }
    if (SceneActorRegistry_CountLivingByCategory(id))
        return 0;
    return 1;
}
