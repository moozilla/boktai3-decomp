#include "scene_actor_contact_list.h"
void SceneActorContacts_RemoveNode(struct SceneActorContactNode *node)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    if (manager) {
        struct SceneActorContactNode *current = manager->head;
        struct SceneActorContactNode *previous = NULL;
        while (current) {
            if (current == node) {
                if (!previous)
                    manager->head = current->next;
                else
                    previous->next = current->next;
                manager->count--;
                return;
            }
            previous = current;
            current = current->next;
        }
    }
}
