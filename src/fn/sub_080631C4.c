#include "scene_inventory_lifecycle.h"
#define SceneInventory_WaitForController sub_080631C4
s32 sub_080631E8(struct SceneInventoryTail *);
void SceneInputPanel_RequestAction(struct SceneInventoryTail *,
                                   s32 (*)(struct SceneInventoryTail *));
s32 SceneInventory_WaitForController(struct SceneInventoryTail *p)
{
    if (((struct SceneInventoryLifecycle *)p)->frame > 31)
        SceneInputPanel_RequestAction(p, sub_080631E8);
    return 0;
}
