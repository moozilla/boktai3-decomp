#include "scene_actor_control.h"
#include "ram.h"
void SceneActorControl_RequestDeparture(s32 index)
{
    struct SceneActorControlManager *manager = gSceneActorControlManager;
    if (index >= 0) {
        struct SceneActorControlSlot *slot = &manager->slots[index];
        if (slot->index >= 0 && slot->kind != 2) {
            slot->flags |= 2;
            slot->collider.flags |= 4;
        }
    }
}
