#include "scene_actor_contacts.h"
#define SceneActorContacts_Update sub_08073B60
u32 SceneActorContacts_Update(struct SceneActorContactManager *manager)
{
    sub_08072D34();
    sub_08073B38();
    return ++manager->frame;
}
