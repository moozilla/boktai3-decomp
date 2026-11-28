#include "scene/collision/scene_actor_contact_list.h"
u8 SceneActorContacts_ContainsActiveNode(struct SceneActorContactNode *input)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *node;
    if (!manager || !input)
        return 0;
    for (node = manager->head; node; node = node->next)
        if ((node->flags & 1) && node == input)
            return 1;
    return 0;
}
