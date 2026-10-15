#include "scene_inventory_lifecycle.h"
s32 SceneInventory_RunController(struct SceneInventoryTail *);
void SceneInputPanel_RequestAction(struct SceneInventoryTail *,
                                   s32 (*)(struct SceneInventoryTail *));
s32 SceneInventory_WaitForController(struct SceneInventoryTail *p)
{
    if (((struct SceneInventoryLifecycle *)p)->frame > 31)
        SceneInputPanel_RequestAction(p, SceneInventory_RunController);
    return 0;
}
