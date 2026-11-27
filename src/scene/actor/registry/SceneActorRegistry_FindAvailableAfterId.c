#include "scene/actor/scene_actor_registry.h"
struct ChaseActor *SceneActorRegistry_FindAvailableAfterId(u32 id)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct ChaseActor *actor = node->actor;
    while (actor) {
        u32 offset = 0x2cc;
        u32 *field;
        field = (void *)((u8 *)actor - (-offset));
        if (!(*field & 1) && !id)
            return actor;
        if (actor->base.body.id == id)
            id = 0;
        node = node->next;
        actor = node->actor;
    }
    return NULL;
}
