#include "scene_actor_registry.h"
#define SceneActorRegistry_CreateManager sub_08071D9C
struct SceneActorRegistryManager *SceneActorRegistry_CreateManager(u32 resourceId, u32 context)
{
    u32 resource = (u16)resourceId, secondary = (u16)context;
    struct SceneActorRegistryManager *manager;
    if (gUnk_020004A8)
        return gUnk_020004A8;
    {
        manager = Task_Create(8, sizeof(*manager));
        if (manager) {
            Task_SetCallbacks(manager, (void (*)(void *))sub_08071CFC,
                              (void (*)(void *))sub_08071D3C);
            if (sub_08071D60(manager, resource, secondary) < 0) {
                Task_Kill(manager);
                return 0;
            }
        }
    }
    return manager;
}
