#include "scene_inventory_lifecycle.h"
s32 SceneInputPanel_AdjustSlidingY(struct SceneInputWidget *, s32, u16);
void SceneInventory_BeginAlternateSelection(struct SceneInventoryTail *);
s32 SceneInventory_TickSaleFeedback(struct SceneInventoryTail *p)
{
    u16 *frame;
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    frame = &((struct SceneInventoryLifecycle *)p)->frame;
    if (*frame == 0)
        Sound_PlaySong(630);
    if (SceneInputPanel_AdjustSlidingY(&p->display.buttons[0], 32, *frame))
        SceneInventory_BeginAlternateSelection(p);
    return 0;
}
