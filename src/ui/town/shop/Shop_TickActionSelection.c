#include "ui/town/shop_tail.h"
s32 Shop_TickActionSelection(struct ShopTail *p)
{
    SceneInputPanel_HighlightCompare(&p->display, p->highlight.colors);
    if (Shop_HandleClockShortcut(p))
        return 0;
    if (Shop_HandlePageKeys(p)) {
        Shop_UpdateModePrompt(p, 1);
        return 0;
    }
    if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        Shop_BeginBrowse(p);
    } else if (1 & gPadState[1]) {
        switch (p->category) {
            case 0:
                Shop_ConfirmEquipmentAction(p);
                break;
            case 1:
                Shop_ConfirmItemAction(p);
                break;
            case 2:
                Shop_ConfirmConsumableAction(p);
                break;
        }
    } else if (SceneInput_NavigateGrid(&p->grid)) {
        p->interactionFlags |= 0x80;
        Shop_UpdateModePrompt(p, 1);
    }
    return 0;
}
