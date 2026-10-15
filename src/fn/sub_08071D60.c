#include "scene_actor_registry.h"
void sub_0806FAD4(struct SceneActorRegistryManager *);
u32 *Memory_ClearWordPair(u32 *);
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
    /* The caller tests this retained subsystem address as a signed status. */
    return (s32)Memory_ClearWordPair(manager->subsystem);
}
