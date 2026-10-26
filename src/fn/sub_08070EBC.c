#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32 id)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct SceneActorRegistryActor *actor;
    while ((actor = node->actor) != NULL) {
        if (actor->id == id)
            return actor;
        node = node->next;
    }
    return NULL;
}
