#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_CreateManager(u32 resourceId, u32 context)
{
    u32 resource = (u16)resourceId, secondary = (u16)context;
    struct SceneActorRegistryManager *manager;
    if (gSceneActorRegistryManager)
        return gSceneActorRegistryManager;
    {
        manager = Task_Create(8, sizeof(*manager));
        if (manager) {
            Task_SetCallbacks(manager, (void (*)(void *))SceneActorRegistry_TickManager,
                              (void (*)(void *))SceneActorRegistry_DestroyManager);
            if (SceneActorRegistry_InitializeManager(manager, resource, secondary) < 0) {
                Task_Kill(manager);
                return 0;
            }
        }
    }
    return manager;
}
