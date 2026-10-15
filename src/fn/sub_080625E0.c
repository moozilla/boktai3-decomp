#include "scene_inventory_tail.h"
#define SceneInventory_TickSaleConfirmation sub_080625E0
void sub_08061A40(struct SceneInventoryTail *);
void sub_08061B48(struct SceneInventoryTail *);
s32 SceneInventory_TickSaleConfirmation(struct SceneInventoryTail *p)
{
    s32 result;
    SceneInputPanel_HighlightPrompt(p, &p->highlight);
    result = SceneInput_TickChoice(&p->display.prompt, &p->display.widgetResource, &p->choiceState,
                                   &p->choiceRepeat);
    if (result == 1)
        sub_08061A40(p);
    else if (result == 0) {
        Sound_PlaySong(0xDE);
        sub_08061B48(p);
    }
    return 0;
}
