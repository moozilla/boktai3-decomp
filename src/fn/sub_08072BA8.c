#include "scene_actor_contact_list.h"
#define SceneActorContacts_ResetList sub_08072BA8
void SceneActorContacts_ResetList(void)
{
    struct SceneActorContactManager *manager = gUnk_020001A0;
    if (manager) {
        manager->count = 0;
        manager->frame = 0;
        manager->field18 = 0;
        manager->head = NULL;
    }
}
