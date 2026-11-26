#include "objects/sliding_doors/scene_object_controller.h"

void SlidingDoors_SetInactive(struct SceneObjPool *pool, u16 id)
{
    int i;
    for (i = 0; i < pool->count; i++) {
        struct SceneObjSlot *slot = &pool->slots[i];
        if (slot->id == id)
            SceneObjSlot_SetAnimation(slot, SCENE_OBJ_INACTIVE, SlidingDoors_UpdateInactiveIdle, 2);
    }
}
