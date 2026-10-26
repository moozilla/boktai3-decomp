#include "scene_actor_contact_list.h"
u8 SceneActorContacts_RegisterNode(struct SceneActorContactNode *node, void *object, u32 kind)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *current;
    if (!manager || (s32)manager->count > 5)
        return 0;
    current = manager->head;
    while (current) {
        if (current == node)
            return 0;
        current = current->next;
    }
    node->object = object;
    node->kind = kind;
    node->flags = 1;
    node->cooldown = 0;
    node->owner = NULL;
    node->next = manager->head;
    manager->head = node;
    manager->count++;
    return 1;
}
