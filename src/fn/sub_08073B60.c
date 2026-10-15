#include "scene_actor_contacts.h"
u32 SceneActorContacts_Update(struct SceneActorContactManager *manager)
{
    SceneActorContacts_ReleaseInvalidOwners();
    SceneActorContacts_TickCooldowns();
    return ++manager->frame;
}
