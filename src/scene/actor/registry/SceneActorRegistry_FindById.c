#include "scene/actor/scene_actor_registry.h"
struct ChaseActor *SceneActorRegistry_FindById(u32 id)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct ChaseActor *actor;
    while ((actor = node->actor) != NULL) {
        if (actor->base.body.id == id)
            return actor;
        node = node->next;
    }
    return NULL;
}
