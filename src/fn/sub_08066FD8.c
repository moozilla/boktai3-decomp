#include "scene_actor_control.h"
#include "ram.h"
/* SceneActorControl_RequestJitter: Sets the jitter request flag on a valid slot unless it is departing */
#define SceneActorControl_RequestJitter sub_08066FD8
#define SceneActorControl_RequestJitter sub_08066FD8
void SceneActorControl_RequestJitter(s32 index)
{
    struct SceneActorControlManager *manager = gSceneActorControlManager;
    if (index >= 0) {
        struct SceneActorControlSlot *slot = &manager->slots[index];
        if (slot->index >= 0 && slot->kind != 2)
            slot->flags |= 4;
    }
}
