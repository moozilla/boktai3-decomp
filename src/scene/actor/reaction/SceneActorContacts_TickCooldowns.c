#include "scene/collision/scene_actor_contacts.h"
void SceneActorContacts_TickCooldowns(void)
{
    if (gSceneActorContactManager) {
        struct SceneActorContactNode *node;
        for (node = gSceneActorContactManager->head; node; node = node->next)
            if (node->cooldown)
                node->cooldown--;
    }
}
