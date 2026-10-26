#include "scene_actor_palette.h"
/* SceneActorPalette_IsTransitionComplete: Reports completion when transitions are disabled or no relevant living actors remain; does not inspect frame counters */
#define SceneActorPalette_IsTransitionComplete sub_08072124
#define SceneActorPalette_IsTransitionComplete sub_08072124

static inline u8 Test(u32 v, u32 mask)
{
    if (v & mask)
        return TRUE;
    return FALSE;
}

static inline u8 TestP(u32 mask, u32 *p)
{
    if (*p & mask)
        return TRUE;
    return FALSE;
}

u32 SceneActorPalette_IsTransitionComplete(void)
{
    u32 id;
    struct SceneActorPaletteManagerView *manager = (void *)gSceneActorRegistryManager;
    if (manager == NULL || !TestP(0x2000, &manager->flags))
        return 1;
    if (Test(gEngineFlags, 0x4800))
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
