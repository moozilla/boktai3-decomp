#include "scene_actor_contact_list.h"
#define SceneActorContacts_ReleaseNode sub_08072E08
void SceneActorContacts_ReleaseNode(struct SceneActorContactNode *input)
{
    struct SceneActorContactManager *manager = gUnk_020001A0;
    struct SceneActorContactNode *node;
    if (!manager || !input)
        return;
    for (node = manager->head; node; node = node->next) {
        if (node == input && (node->flags & 1)) {
            node->flags &= 0xFD;
            node->owner = NULL;
            node->cooldown = 240;
            return;
        }
    }
}
