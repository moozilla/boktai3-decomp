#include "scene_actor_registry.h"
void SceneActorRegistry_InitializeList(struct SceneActorRegistryManager *);
u32 *Memory_ClearWordPair(u32 *);
s32 SceneActorRegistry_InitializeManager(struct SceneActorRegistryManager *manager, u32 resourceId,
                                         u32 unusedContext)
{
    manager->resourceId = (u16)resourceId;
    manager->frame = 0;
    manager->field32 = 0;
    manager->field62 = 0;
    manager->mode = 3;
    SceneActorRegistry_InitializeList(manager);
    gSceneActorRegistryManager = manager;
    /* The caller tests this retained subsystem address as a signed status. */
    return (s32)Memory_ClearWordPair(manager->subsystem);
}
