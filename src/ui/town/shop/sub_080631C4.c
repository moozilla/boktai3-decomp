#include "ui/town/shop_lifecycle.h"
s32 Shop_RunController(struct SceneInventoryTail *);
void SceneInputPanel_RequestAction(struct SceneInventoryTail *,
                                   s32 (*)(struct SceneInventoryTail *));
s32 Shop_WaitForController(struct SceneInventoryTail *p)
{
    if (((struct SceneInventoryLifecycle *)p)->frame > 31)
        SceneInputPanel_RequestAction(p, Shop_RunController);
    return 0;
}
