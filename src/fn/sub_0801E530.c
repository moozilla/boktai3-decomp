#include "scene_object_controller.h"

void SceneObject_AnimateActive(struct SceneObjSlot *);

void SceneObjects_BeginActivation(struct SceneObjPool *pool, u16 id)
{
    int i;
    for (i = 0; i < pool->count; i++) {
        struct SceneObjSlot *slot = &pool->slots[i];
        if (slot->id == id &&
            (slot->variant == SCENE_OBJ_INACTIVE || slot->variant == SCENE_OBJ_DEACTIVATING) &&
            !(slot->kind & 1)) {
            slot->b41 = 1;
            SceneObjSlot_SetAnimation(slot, SCENE_OBJ_ACTIVATING, SceneObject_AnimateActive, 0);
        }
    }
}
