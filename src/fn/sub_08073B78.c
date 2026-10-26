#include "scene_actor_contacts.h"
struct SceneActorContactManager *
SceneActorContacts_Destroy(struct SceneActorContactManager *unusedManager)
{
    gSceneActorContactManager = 0;
    return unusedManager;
}
