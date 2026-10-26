#include "scene_actor_registry.h"
void SceneActorProfiles_FreeAll(void);
s32 SceneActorRegistry_DestroyManager(struct SceneActorRegistryManager *unusedManager)
{
    SceneActorRegistry_DestroyActors();
    Heap_Free(gSceneActorRegistryNode);
    SceneActorProfiles_FreeAll();
    gSceneActorRegistryManager = 0;
    return 0;
}
