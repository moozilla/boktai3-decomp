#include "scene_inventory_tail.h"
#define SceneInventory_TickActionSelection sub_080620D4
s32 SceneInventory_TickActionSelection(struct SceneInventoryTail *p)
{
    SceneInputPanel_HighlightCompare(&p->display, p->highlight.colors);
    if (sub_08061C3C(p))
        return 0;
    if (sub_08061CFC(p)) {
        sub_080613BC(p, 1);
        return 0;
    }
    if (gUnk_03005260[1] & 2) {
        Sound_PlaySong(0xDE);
        sub_08061540(p);
    } else if (1 & gUnk_03005260[1]) {
        switch (p->category) {
            case 0:
                sub_08061F18(p);
                break;
            case 1:
                sub_08061FAC(p);
                break;
            case 2:
                sub_08062040(p);
                break;
        }
    } else if (SceneInput_NavigateGrid(&p->grid)) {
        p->interactionFlags |= 0x80;
        sub_080613BC(p, 1);
    }
    return 0;
}
