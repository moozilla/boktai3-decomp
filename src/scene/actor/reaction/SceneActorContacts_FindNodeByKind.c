#include "scene/collision/scene_actor_contact_list.h"
struct SceneActorContactNode *SceneActorContacts_FindNodeByKind(u32 kind)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *node;
    if (!manager)
        return NULL;
    for (node = manager->head; node; node = node->next)
        if (node->kind == kind)
            return node;
    return NULL;
}
