#include "objects/sliding_doors/scene_object_controller.h"

void SlidingDoors_ScriptPromote(void)
{
    struct SceneObjPool *pool = ResourceRegistry_GetResource(0x85e6);
    int id;
    int i;

    if (!pool || !Script_SeekToKeyword(0x6e))
        return;
    id = Script_GetValue();
    for (i = 0; i < pool->count; i++) {
        struct SceneObjSlot *slot = &pool->slots[i];
        if (slot->id == id) {
            slot->autoActivate = 1;
            pool->primary[pool->primaryCount] = slot->id;
            pool->primaryCount++;
            SceneObjSlot_ApplyActivation(slot, SCENE_OBJ_ACTIVATING);
        }
    }
}
