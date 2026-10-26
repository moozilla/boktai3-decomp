#include "scene_actor_contacts.h"
void SceneActorContacts_TickCooldowns(void)
{
    if (gSceneActorContactManager) {
        struct SceneActorContactNode *node = gSceneActorContactManager->head;
        while (node) {
            u16 cooldown = node->cooldown;
            if (cooldown)
                node->cooldown = cooldown - 1;
            node = node->next;
        }
    }
}
