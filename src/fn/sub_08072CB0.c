#include "scene_actor_contact_list.h"
u8 SceneActorContacts_ContainsActiveNode(struct SceneActorContactNode *input)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *node;
    if (!manager)
        goto failure;
    if (input)
        goto search;
    goto failure;
found:
    return 1;
search:
    for (node = manager->head; node; node = node->next)
        if ((node->flags & 1) && node == input)
            goto found;
failure:
    return 0;
}
