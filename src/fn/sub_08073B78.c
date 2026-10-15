#include "scene_actor_contacts.h"
#define SceneActorContacts_Destroy sub_08073B78
struct SceneActorContactManager *
SceneActorContacts_Destroy(struct SceneActorContactManager *unusedManager)
{
    gUnk_020001A0 = 0;
    return unusedManager;
}
