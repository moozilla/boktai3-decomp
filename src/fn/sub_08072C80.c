#include "scene_actor_contact_list.h"
#define SceneActorContacts_FindNodeByKind sub_08072C80
struct SceneActorContactNode *SceneActorContacts_FindNodeByKind(u32 kind)
{
    struct SceneActorContactManager *manager = gUnk_020001A0;
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
