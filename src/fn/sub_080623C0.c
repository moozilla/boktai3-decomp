#include "scene_inventory_tail.h"
#define SceneInventory_TickPurchaseConfirmation sub_080623C0
void sub_080619D4(struct SceneInventoryTail *);
void sub_08061B48(struct SceneInventoryTail *);
s32 SceneInventory_TickPurchaseConfirmation(struct SceneInventoryTail *p)
{
    s32 result;
    SceneInputPanel_HighlightPrompt(p, &p->highlight);
    result = SceneInput_TickChoice(&p->display.prompt, &p->display.widgetResource, &p->choiceState,
                                   &p->choiceRepeat);
    if (result == 1)
        sub_080619D4(p);
    else if (result == 0) {
        Sound_PlaySong(0xDE);
        sub_08061B48(p);
    }
    return 0;
}
