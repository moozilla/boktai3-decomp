#include "scene_inventory_tail.h"
#define SceneInventory_TickMoveSelection sub_08062190
s32 SceneInventory_TickMoveSelection(struct SceneInventoryTail *p)
{
    s32 result;
    SceneInputPanel_HighlightCompare(&p->display, p->highlight.colors);
    if (gUnk_03005260[1] & 2) {
        Sound_PlaySong(0xDE);
        switch (p->mode) {
            case 1:
                sub_08061B94(p);
                break;
            case 4:
            case 5:
                sub_08061B48(p);
                break;
        }
    } else if (gUnk_03005260[1] & 1) {
        result = 0;
        switch (p->category) {
            case 0:
                result = sub_08060864(p);
                break;
            case 1:
                result = sub_08061000(p);
                break;
            case 2:
                result = sub_08061080(p);
                break;
        }
        switch (result) {
            case 1:
                sub_08061A98(p);
                break;
            case 4:
                Sound_PlaySong(0x107);
                sub_0806167C(p);
                break;
            case 5:
                Sound_PlaySong(0x107);
                sub_08061780(p);
                break;
            default:
                Sound_PlaySong(0x192);
                break;
        }
    } else if (SceneInput_NavigateGrid(&p->grid)) {
        p->interactionFlags |= 0x80;
    }
    return 0;
}
