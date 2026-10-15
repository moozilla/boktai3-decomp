#include "scene_inventory_lifecycle.h"
s32 SceneInputPanel_AdjustSlidingY(struct SceneInputWidget *, s32, u16);
void SceneInventory_BeginAlternateConsumableSelection(struct SceneInventoryTail *);
s32 SceneInventory_TickDiscardFeedback(struct SceneInventoryTail *p)
{
    u16 *frame;
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    frame = &((struct SceneInventoryLifecycle *)p)->frame;
    if (*frame == 0)
        Sound_PlaySong(270);
    if (SceneInputPanel_AdjustSlidingY(&p->display.buttons[3], 104, *frame))
        SceneInventory_BeginAlternateConsumableSelection(p);
    return 0;
}
