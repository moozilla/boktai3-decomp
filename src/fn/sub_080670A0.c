#include "scene_actor_control.h"
#define SceneActorControl_OnContact sub_080670A0
void SceneActorControl_OnContact(void *unused0, void *unused1, struct SceneActorControlSlot *slot)
{
    slot->flags |= 2;
    slot->collider.flags |= 4;
}
