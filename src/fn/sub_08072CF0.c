#include "scene_actor_contact_list.h"
void *SceneActorContacts_FindObjectById(u16 id)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *node;
    void *object;
    if (!manager)
        goto failure;
    if (id)
        goto search;
    goto failure;
found:
    return object;
search:
    for (node = manager->head; node; node = node->next) {
        if (node->flags & 1) {
            object = node->object;
            if (*(u32 *)object == id)
                goto found;
        }
    }
failure:
    return NULL;
}
