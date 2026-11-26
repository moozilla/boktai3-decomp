#include "ui/town/shop_tail.h"
s32 Shop_TickMoveSelection(struct SceneInventoryTail *p)
{
    s32 result;
    SceneInputPanel_HighlightCompare(&p->display, p->highlight.colors);
    if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        switch (p->mode) {
            case 1:
                Shop_BeginAlternateItemSelection(p);
                break;
            case 4:
            case 5:
                Shop_BeginAlternateSelection(p);
                break;
        }
    } else if (gPadState[1] & 1) {
        result = 0;
        switch (p->category) {
            case 0:
                result = Shop_ClassifyEquipmentMove(p);
                break;
            case 1:
                result = Shop_ClassifyItemMove(p);
                break;
            case 2:
                result = Shop_ClassifyConsumableMove(p);
                break;
        }
        switch (result) {
            case 1:
                Shop_BeginSwapTransition(p);
                break;
            case 4:
                Sound_PlaySong(0x107);
                Shop_BeginPurchaseConfirmation(p);
                break;
            case 5:
                Sound_PlaySong(0x107);
                Shop_BeginSaleConfirmation(p);
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
