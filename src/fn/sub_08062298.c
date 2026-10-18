#include "scene_inventory_tail.h"
void sub_0822DA94(u32);
void Inventory_SortConsumables(u32);
void GameState_SortConsumableSlots(u32);
s32 SceneInventory_TickResetConfirmation(struct SceneInventoryTail *p)
{
    s32 result;
    SceneInputPanel_HighlightPrompt(&p->display, p->highlight.colors);
    result = SceneInput_TickChoice(&p->display.prompt, &p->display.widgetResource, &p->choiceState,
                                   &p->choiceRepeat);
    if (result == 1) {
        Sound_PlaySong(0xDD);
        switch (p->category) {
            case 0:
                sub_0822DA94(0);
                break;
            case 1:
                Inventory_SortConsumables(0);
                break;
            case 2:
                GameState_SortConsumableSlots(0);
                break;
        }
        SceneInventory_BeginBrowse(p);
    } else if (result == 0) {
        Sound_PlaySong(0xDE);
        SceneInventory_BeginBrowse(p);
    }
    return 0;
}
