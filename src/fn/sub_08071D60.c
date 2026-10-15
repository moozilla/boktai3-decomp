#include "scene_actor_registry.h"
#define SceneActorRegistry_InitializeManager sub_08071D60
void sub_0806FAD4(struct SceneActorRegistryManager *);
s32 sub_08073BFC(void *);
s32 SceneActorRegistry_InitializeManager(struct SceneActorRegistryManager *manager, u32 resourceId,
                                         u32 unusedContext)
{
    manager->resourceId = (u16)resourceId;
    manager->frame = 0;
    manager->field32 = 0;
    manager->field62 = 0;
    manager->mode = 3;
    sub_0806FAD4(manager);
    gUnk_020004A8 = manager;
    return sub_08073BFC(manager->subsystem);
}
