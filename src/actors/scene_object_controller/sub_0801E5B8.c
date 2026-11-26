#include "actors/scene_object_controller/scene_object_controller.h"

void SceneObjects_BeginDeactivation(struct SceneObjPool *pool, u16 id)
{
    int i;
    for (i = 0; i < pool->count; i++) {
        struct SceneObjSlot *slot = &pool->slots[i];
        if (slot->id == id && (u8)(slot->variant - SCENE_OBJ_ACTIVE) <= 1)
            SceneObjSlot_ApplyActivation(slot, SCENE_OBJ_DEACTIVATING);
    }
}
