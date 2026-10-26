#include "scene_actor_registry.h"
s32 OrbitBackground_IsEnabled(void);
void SceneActorRegistry_UpdateBackgroundFlag(struct SceneActorRegistryManager *manager)
{
    if (!(manager->frame & 15)) {
        if (!OrbitBackground_IsEnabled()) {
            u32 mask = 0x20;
            u32 *flags = &gGameState->flags868;
            *flags |= mask;
        } else {
            u32 *flags = &gGameState->flags868;
            u32 mask = ~0x20;
            *flags &= mask;
        }
    }
}
