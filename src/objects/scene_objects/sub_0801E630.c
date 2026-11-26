#include "objects/scene_objects/scene_object_controller.h"

void SceneObjects_SetActive(struct SceneObjPool *pool, u16 id)
{
    int i;
    for (i = 0; i < pool->count; i++) {
        struct SceneObjSlot *slot = &pool->slots[i];
        if (slot->id == id)
            SceneObjSlot_ApplyActivation(slot, SCENE_OBJ_ACTIVE);
    }
}
