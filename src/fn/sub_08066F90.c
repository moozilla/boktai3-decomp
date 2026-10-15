#include "scene_actor_control.h"
#define SceneActorControl_RequestDeparture sub_08066F90
extern struct SceneActorControlManager *gUnk_02000128;
void SceneActorControl_RequestDeparture(s32 index)
{
    struct SceneActorControlManager *manager = gUnk_02000128;
    if (index >= 0) {
        struct SceneActorControlSlot *slot = &manager->slots[index];
        if (slot->index >= 0 && slot->kind != 2) {
            slot->flags |= 2;
            slot->collider.flags |= 4;
        }
    }
}
