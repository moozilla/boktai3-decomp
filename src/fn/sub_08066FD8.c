#include "scene_actor_control.h"
#define SceneActorControl_RequestJitter sub_08066FD8
extern struct SceneActorControlManager *gUnk_02000128;
void SceneActorControl_RequestJitter(s32 index)
{
    struct SceneActorControlManager *manager = gUnk_02000128;
    if (index >= 0) {
        struct SceneActorControlSlot *slot = &manager->slots[index];
        if (slot->index >= 0 && slot->kind != 2)
            slot->flags |= 4;
    }
}
