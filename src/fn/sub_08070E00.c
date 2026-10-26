#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindAvailableAfterId(u32 id)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct SceneActorRegistryActor *actor = node->actor;
    while (actor) {
        u32 offset = 0x2cc;
        u32 *field;
        field = (void *)((u8 *)actor - (-offset));
        if (!(*field & 1) && !id)
            return actor;
        if (actor->id == id)
            id = 0;
        node = node->next;
        actor = node->actor;
    }
    return NULL;
}
