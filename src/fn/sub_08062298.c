#include "scene_inventory_tail.h"
#define SceneInventory_TickResetConfirmation sub_08062298
void sub_0822DA94(u32);
void sub_0822D0F0(u32);
void sub_0822E18C(u32);
s32 SceneInventory_TickResetConfirmation(struct SceneInventoryTail *p)
{
    s32 result;
    SceneInputPanel_HighlightPrompt(p, &p->highlight);
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
        sub_08061540(p);
    } else if (result == 0) {
        Sound_PlaySong(0xDE);
        sub_08061540(p);
    }
    return 0;
}
