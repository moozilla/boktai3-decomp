#include "scene_inventory_tail.h"
void SceneInventory_BeginSpecialTransition(struct SceneInventoryTail *);
void SceneInventory_BeginAlternateSelection(struct SceneInventoryTail *);
s32 SceneInventory_TickSaleConfirmation(struct SceneInventoryTail *p)
{
    s32 result;
    SceneInputPanel_HighlightPrompt(&p->display, p->highlight.colors);
    result = SceneInput_TickChoice(&p->display.prompt, &p->display.widgetResource, &p->choiceState,
                                   &p->choiceRepeat);
    if (result == 1)
        SceneInventory_BeginSpecialTransition(p);
    else if (result == 0) {
        Sound_PlaySong(0xDE);
        SceneInventory_BeginAlternateSelection(p);
    }
    return 0;
}
