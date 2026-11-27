#include "ui/town/shop_tail.h"
#include "ui/town/shop_item_values.h"
struct ShopSaleView {
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
s32 Shop_TickSaleFeedback(struct ShopTail *);
s32 Shop_TickSaleTransition(struct ShopTail *p)
{
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct ShopSaleView *)p)->pair)) {
        u32 selection = p->grid.savedSelection;
        s32 category = p->category;
        s32 value;
        switch (category) {
            case 0: {
                s16 *funds;
                value = SceneItemValue_CalculateEquipment(selection);
                funds = &gGameState->unk7B0;
                *funds += value;
                if (gGameState->unk7B0 > 9999)
                    gGameState->unk7B0 = 9999;
                Inventory_RemoveEquipmentEntry(selection);
                break;
            }
            case 1: {
                s16 *funds;
                s32 item;
                value = SceneItemValue_CalculateItem(selection);
                funds = &gGameState->unk7B0;
                *funds += value;
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
                s16 *funds;
                value = SceneItemValue_CalculateConsumable(selection);
                funds = &gGameState->unk7B0;
                *funds += value;
                if (gGameState->unk7B0 > 9999)
                    gGameState->unk7B0 = 9999;
                GameState_ClearConsumableSlot(selection);
                break;
            }
        }
        p->sold = 1;
        Shop_SetControllerCallback(
            p, (ShopCallback)Shop_TickSaleFeedback);
    }
    return 0;
}
