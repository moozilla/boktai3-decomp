#include "scene_actor_registry.h"
void SceneActorRegistry_InitializeList(struct SceneActorRegistryManager *manager)
{
    struct SceneActorRegistryNode *node;
    manager->sentinel = Heap_Alloc(8);
    Memory_Clear(manager->sentinel, 8);
    node = manager->sentinel;
    node->next = 0;
    node->actor = 0;
    gSceneActorRegistryNode = node;
    manager->cursor = node;
}
