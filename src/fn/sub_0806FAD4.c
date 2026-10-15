#include "scene_actor_registry.h"
#define SceneActorRegistry_InitializeList sub_0806FAD4
void SceneActorRegistry_InitializeList(struct SceneActorRegistryManager *manager)
{
    struct SceneActorRegistryNode *node;
    manager->sentinel = Heap_Alloc(8);
    Memory_Clear(manager->sentinel, 8);
    node = manager->sentinel;
    node->next = 0;
    node->actor = 0;
    gUnk_020004AC = node;
    manager->cursor = node;
}
