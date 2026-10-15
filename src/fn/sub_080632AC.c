#include "scene_inventory_lifecycle.h"
#define SceneInventory_TickController sub_080632AC
void SceneHud_DrawNumericStatus(u8);
void SceneHud_DrawClock(u8);
void sub_0824923C(void *, void *);
s32 SceneInventory_TickController(struct SceneInventoryLifecycleView *owner)
{
    u8 *mode = &owner->clockMode;
    SceneHud_DrawNumericStatus(*mode);
    SceneHud_DrawClock(*mode);
    sub_0824923C(owner, owner->action);
    owner->frame++;
    return 0;
}
