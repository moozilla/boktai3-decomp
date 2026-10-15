#include "scene_inventory_tail.h"
#define SceneInventory_TickDiscardConfirmation sub_080627C4
void sub_08061AF0(struct SceneInventoryTail *);
void sub_08061C04(struct SceneInventoryTail *);
s32 SceneInventory_TickDiscardConfirmation(struct SceneInventoryTail *p)
{
    s32 result;
    SceneInputPanel_HighlightPrompt(p, &p->highlight);
    result = SceneInput_TickChoice(&p->display.prompt, &p->display.widgetResource, &p->choiceState,
                                   &p->choiceRepeat);
    if (result == 1)
        sub_08061AF0(p);
    else if (result == 0) {
        Sound_PlaySong(0xDE);
        sub_08061C04(p);
    }
    return 0;
}
