#include "scene_actor_contact_list.h"
void SceneActorContacts_ResetList(void)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    if (manager) {
        manager->count = 0;
        manager->frame = 0;
        manager->field18 = 0;
        manager->head = NULL;
    }
}
