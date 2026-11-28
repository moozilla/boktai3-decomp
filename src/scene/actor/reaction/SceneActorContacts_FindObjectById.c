#include "scene/collision/scene_actor_contact_list.h"
void *SceneActorContacts_FindObjectById(u16 id)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *node;
    if (!manager || !id)
        return NULL;
    for (node = manager->head; node; node = node->next) {
        if (node->flags & 1) {
            void *object = node->object;
            if (*(u32 *)object == id)
                return object;
        }
    }
    return NULL;
}
