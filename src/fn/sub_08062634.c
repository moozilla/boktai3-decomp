// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_tail.h"
#include "scene_item_values.h"
struct SceneInventorySaleView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
struct SceneInventorySaleFlags {
    u8 unknown0[0x1AAD];
    u8 sold;
};
struct SceneInventoryMoney {
    u8 unknown0[0x7B0];
    s16 funds;
};
struct SceneInventoryUnlocks {
    u8 unknown0[0x750];
    u32 low, high;
};
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
s32 SceneItemValue_CalculateEquipment(u32);
s32 SceneItemValue_CalculateItem(u32);
s32 SceneItemValue_CalculateConsumable(u32);
void Inventory_RemoveEquipmentEntry(u32);
void Inventory_ClearItemSlot(u32);
void GameState_ClearConsumableSlot(u32);
s32 SceneInventory_TickSaleFeedback(struct SceneInventoryTail *);
static inline void AddFunds(s32 value, struct GameState **location)
{
    register u32 offset asm("r2");
    struct SceneInventoryMoney *first = (void *)*location;
    struct SceneInventoryMoney *second;
    offset = 0x7B0;
    first = (void *)((u8 *)first + offset);
    *(u16 *)first += value;
    second = (void *)((u8 *)*(struct GameState *volatile *)location + offset);
    if (*(s16 *)second > 9999)
        *(s16 *)second = 9999;
}
s32 SceneInventory_TickSaleTransition(struct SceneInventoryTail *input)
{
    struct SceneInventoryTail *p = input;
    u32 selection;
    s32 category;
    register s32 value asm("r5");
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct SceneInventorySaleView *)p)->pair)) {
        selection = p->grid.savedSelection;
        category = p->category;
        switch (category) {
            case 0:
                value = SceneItemValue_CalculateEquipment(selection);
                asm("" : "+r"(value));
                AddFunds(value, &gGameState);
                Inventory_RemoveEquipmentEntry(selection);
                break;
            case 1: {
                register s32 item asm("r0");
                u32 *bits;
                u32 offset;
                struct GameState **location;
                value = SceneItemValue_CalculateItem(selection);
                asm("" : "+r"(value));
                location = &gGameState;
                AddFunds(value, location);
                item = Inventory_GetItemId(selection);
                if (item <= 31) {
                    bits = (u32 *)*location;
                    offset = 0x750;
                    bits = (u32 *)((u8 *)bits + offset);
                } else {
                    bits = (u32 *)*location;
                    offset = 0x754;
                    bits = (u32 *)((u8 *)bits + offset);
                    item -= 32;
                }
                category = (u32)category << item;
                item = *bits;
                item |= category;
                *bits = item;
                Inventory_ClearItemSlot(selection);
                break;
            }
            case 2:
                value = SceneItemValue_CalculateConsumable(selection);
                asm("" : "+r"(value));
                AddFunds(value, &gGameState);
                GameState_ClearConsumableSlot(selection);
                break;
        }
        ((struct SceneInventorySaleFlags *)p)->sold = 1;
        SceneInventory_SetControllerCallback(
            p, (SceneInventoryCallback)SceneInventory_TickSaleFeedback);
    }
    return 0;
}
