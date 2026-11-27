#include "ui/town/shop_lifecycle.h"
s32 Shop_RunController(struct ShopTail *);
void SceneInputPanel_RequestAction(struct ShopTail *,
                                   s32 (*)(struct ShopTail *));
s32 Shop_WaitForController(struct ShopTail *p)
{
    if (((struct ShopLifecycle *)p)->frame > 31)
        SceneInputPanel_RequestAction(p, Shop_RunController);
    return 0;
}
