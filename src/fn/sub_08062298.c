#include "scene_inventory_tail.h"
void sub_0822DA94(u32);
void sub_0822D0F0(u32);
void sub_0822E18C(u32);
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
                sub_0822D0F0(0);
                break;
            case 2:
                sub_0822E18C(0);
                break;
        }
        SceneInventory_BeginBrowse(p);
    } else if (result == 0) {
        Sound_PlaySong(0xDE);
        SceneInventory_BeginBrowse(p);
    }
    return 0;
}
