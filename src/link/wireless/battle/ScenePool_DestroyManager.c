#include "link/wireless/scene_pool.h"
#include "engine/ram.h"

void ScenePool_DestroyActorSlot(struct ScenePoolManager *, struct ScenePoolActor *);
void HashTable_Destroy(u32 *);
void Heap_Free(void *);

u32 ScenePool_DestroyManager(struct ScenePoolManager *manager)
{
    struct ScenePoolActor *actor = manager->actors;
    s32 i = 3;
    u32 zero;
    do {
        ScenePool_DestroyActorSlot(manager, actor);
        actor++;
        i--;
    } while (i >= 0);
    zero = 0;
    HashTable_Destroy(manager->registry);
    Heap_Free(manager->entries);
    manager->entries = (void *)zero;
    gScenePool = zero;
    return 0;
}
