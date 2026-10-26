#include "scene_actor_contact_list.h"
struct SceneActorContactNode *SceneActorContacts_FindNodeByKind(u32 kind)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *node;
    if (manager)
        goto search;
    goto failure;
found:
    return node;
search:
    for (node = manager->head; node; node = node->next)
        if (node->kind == kind)
            goto found;
failure:
    return NULL;
}
