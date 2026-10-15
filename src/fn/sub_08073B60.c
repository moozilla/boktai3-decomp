#include "scene_actor_contacts.h"
u32 SceneActorContacts_Update(struct SceneActorContactManager *manager)
{
    sub_08072D34();
    SceneActorContacts_TickCooldowns();
    return ++manager->frame;
}
