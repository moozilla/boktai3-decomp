#include "scene_inventory_lifecycle.h"
#define SceneInventory_TickDiscardFeedback sub_0806288C
s32 SceneInputPanel_AdjustSlidingY(struct SceneInputWidget *, s32, u16);
void sub_08061C04(struct SceneInventoryTail *);
s32 SceneInventory_TickDiscardFeedback(struct SceneInventoryTail *p)
{
    u16 *frame;
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    frame = &((struct SceneInventoryLifecycle *)p)->frame;
    if (*frame == 0)
        Sound_PlaySong(270);
    if (SceneInputPanel_AdjustSlidingY(&p->display.buttons[3], 104, *frame))
        sub_08061C04(p);
    return 0;
}
