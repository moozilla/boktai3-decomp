#include "scene_actor_registry.h"
#define SceneActorRegistry_FindById sub_08070EBC
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32 id)
{
    struct SceneActorRegistryNode *node = gUnk_020004AC;
    struct SceneActorRegistryActor *actor;
    while ((actor = node->actor) != NULL) {
        if (actor->id == id)
            return actor;
        node = node->next;
    }
    return NULL;
}
