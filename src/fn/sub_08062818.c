#include "scene_inventory_tail.h"
struct SceneInventorySwapView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
void Inventory_RemoveEquipmentEntry(u32);
s32 SceneInventory_TickDiscardFeedback(struct SceneInventoryTail *);
void Inventory_ClearItemSlot(u32);
void GameState_ClearConsumableSlot(u32);
s32 SceneInventory_TickDiscardTransition(struct SceneInventoryTail *p)
{
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct SceneInventorySwapView *)p)->pair)) {
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
        SceneInventory_SetControllerCallback(
            p, (SceneInventoryCallback)SceneInventory_TickDiscardFeedback);
    }
    return 0;
}
