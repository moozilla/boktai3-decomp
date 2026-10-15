#include "scene_actor_registry.h"
void sub_08073C04(void);
s32 SceneActorRegistry_DestroyManager(struct SceneActorRegistryManager *unusedManager)
{
    SceneActorRegistry_DestroyActors();
    Heap_Free(gUnk_020004AC);
    sub_08073C04();
    gUnk_020004A8 = 0;
    return 0;
}
