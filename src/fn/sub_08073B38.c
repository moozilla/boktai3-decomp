#include "scene_actor_contacts.h"
#define SceneActorContacts_TickCooldowns sub_08073B38
void SceneActorContacts_TickCooldowns(void)
{
    if (gUnk_020001A0) {
        struct SceneActorContactNode *node = gUnk_020001A0->head;
        while (node) {
            u16 cooldown = node->cooldown;
            if (cooldown)
                node->cooldown = cooldown - 1;
            node = node->next;
        }
    }
}
