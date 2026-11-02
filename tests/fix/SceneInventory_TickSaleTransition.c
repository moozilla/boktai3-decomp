// draft with the funds update stored through the field in each case. The ROM loads gGameState
// again for the clamp, so the original stored through a pointer (sp_store_via_ptr, README fact 60)
#include "scene_inventory_tail.h"
#include "scene_item_values.h"
struct SceneInventorySaleView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
s32 SceneItemValue_CalculateEquipment(u32);
s32 SceneItemValue_CalculateItem(u32);
s32 SceneItemValue_CalculateConsumable(u32);
void Inventory_RemoveEquipmentEntry(u32);
void Inventory_ClearItemSlot(u32);
void GameState_ClearConsumableSlot(u32);
s32 SceneInventory_TickSaleFeedback(struct SceneInventoryTail *);
s32 SceneInventory_TickSaleTransition(struct SceneInventoryTail *p)
{
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct SceneInventorySaleView *)p)->pair)) {
        u32 selection = p->grid.savedSelection;
        s32 category = p->category;
        s32 value;
        switch (category) {
            case 0: {
                value = SceneItemValue_CalculateEquipment(selection);
                gGameState->unk7B0 += value;
                if (gGameState->unk7B0 > 9999)
                    gGameState->unk7B0 = 9999;
                Inventory_RemoveEquipmentEntry(selection);
                break;
            }
            case 1: {
                s32 item;
                value = SceneItemValue_CalculateItem(selection);
                gGameState->unk7B0 += value;
                if (gGameState->unk7B0 > 9999)
                    gGameState->unk7B0 = 9999;
                item = Inventory_GetItemId(selection);
                if (item < 32)
                    gGameState->unlockedItems[0] |= 1 << item;
                else
                    gGameState->unlockedItems[1] |= 1 << (item - 32);
                Inventory_ClearItemSlot(selection);
                break;
            }
            case 2: {
                value = SceneItemValue_CalculateConsumable(selection);
                gGameState->unk7B0 += value;
                if (gGameState->unk7B0 > 9999)
                    gGameState->unk7B0 = 9999;
                GameState_ClearConsumableSlot(selection);
                break;
            }
        }
        p->sold = 1;
        SceneInventory_SetControllerCallback(
            p, (SceneInventoryCallback)SceneInventory_TickSaleFeedback);
    }
    return 0;
}
