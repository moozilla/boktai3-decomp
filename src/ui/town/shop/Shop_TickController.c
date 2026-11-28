#include "ui/town/shop_lifecycle.h"
void SceneHud_DrawNumericStatus(u8);
void SceneHud_DrawClock(u8);
s32 Shop_TickController(struct ShopLifecycleView *owner)
{
    SceneHud_DrawNumericStatus(owner->clockMode);
    SceneHud_DrawClock(owner->clockMode);
    Callback_InvokeActor(owner, owner->action);
    owner->frame++;
    return 0;
}
