#include "ui/town/shop_tail.h"
struct ShopSwapView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
void Inventory_RemoveEquipmentEntry(u32);
s32 Shop_TickDiscardFeedback(struct ShopTail *);
void Inventory_ClearItemSlot(u32);
void GameState_ClearConsumableSlot(u32);
s32 Shop_TickDiscardTransition(struct ShopTail *p)
{
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct ShopSwapView *)p)->pair)) {
        u32 selection = p->grid.savedSelection;
        switch (p->category) {
            case 0:
                Inventory_RemoveEquipmentEntry(selection);
                break;
            case 1:
                Inventory_ClearItemSlot(selection);
                break;
            case 2:
                GameState_ClearConsumableSlot(selection);
                break;
        }
        Shop_SetControllerCallback(
            p, (ShopCallback)Shop_TickDiscardFeedback);
    }
    return 0;
}
