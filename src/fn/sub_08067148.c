#include "scene_actor_control.h"
struct SceneActorControlSlot *SceneActorControl_AllocateSlot(struct SceneActorControlManager *p)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!SceneActorControl_IsUsed(p, i)) {
            struct SceneActorControlSlot *slot = &p->slots[i];
            p->used |= 1U << i;
            slot->index = i;
            return slot;
        }
    }
    return 0;
}
