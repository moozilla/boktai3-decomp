#include "ui/town/shop_tail.h"
#include "ui/town/shop_catalog.h"
#include "ui/town/shop_item_values.h"
struct ShopPurchaseDescriptor {
    u32 prefix[3];
    u8 modifierA, unknownD[3], modifierB, unknown11[3], modifierC, unknown15[11];
};
struct ShopSwapView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
struct ShopPurchaseFlags {
    u8 unknown0[0x1AAC];
    u8 purchased;
};
struct ShopFunds {
    u8 unknown0[0x7B0];
    s16 funds;
};
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
void Shop_PrepareEquipmentDescriptor(struct ShopTail *,
                                               struct ShopPurchaseDescriptor *);
void Inventory_SetEquipmentEntry(u32, struct ShopPurchaseDescriptor *);
void Inventory_SetItemId(u32, s32);
void Inventory_SetConsumableEntry(u32, s32);
s32 Shop_TickPurchaseTransition(struct ShopTail *p)
{
    struct ShopPurchaseDescriptor descriptor;
    struct ShopScriptCatalog *catalog = (struct ShopScriptCatalog *)p;
    s32 category;
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct ShopSwapView *)p)->pair)) {
        category = p->category;
        switch (category) {
            case 0: {
                u32 destination = p->grid.selection;
                struct SceneValueEquipmentDefinition *definition;
                {
                    s32 index = p->grid.savedSelection - 16;
                    s32 offset;
                    offset = catalog->page * 8;
                    index += offset;
                    offset = (s32)catalog->catalog;
                    offset += index;
                    offset = *(s8 *)offset;
                    definition = Equipment_GetDefinition(offset);
                }
                descriptor = *(struct ShopPurchaseDescriptor *)definition;
                descriptor.modifierA = category;
                descriptor.modifierB = category;
                descriptor.modifierC = category;
                Shop_PrepareEquipmentDescriptor(p, &descriptor);
                Inventory_SetEquipmentEntry(destination, &descriptor);
                Inventory_GetEquipmentEntry(destination)->condition = 500;
                ((struct ShopFunds *)gGameState)->funds -= definition->baseValue;
                break;
            }
            case 1: {
                u32 destination = p->grid.selection;
                s32 index = p->grid.savedSelection - 16;
                u8 *page = &catalog->page;
                s8 *entries;
                s32 offset = index + *page * 8;
                s32 value;
                entries = catalog->catalog;
                value = SceneItemValue_GetItemBaseValue(entries[offset]);
                ((struct ShopFunds *)gGameState)->funds -= value;
                index += *page * 8;
                entries += index;
                Inventory_SetItemId(destination, *entries);
                break;
            }
            case 2: {
                u32 destination = p->grid.selection;
                s32 index = p->grid.savedSelection - 16;
                u8 *page = &catalog->page;
                s8 *entries;
                s32 offset = index + *page * 8;
                s32 value;
                entries = catalog->catalog;
                GameState_GetConsumableDescriptor(entries[offset]);
                offset = index + *page * 8;
                Inventory_SetConsumableEntry(destination, entries[offset]);
                index += *page * 8;
                entries += index;
                value = SceneItemValue_GetConsumableBaseValue(*entries);
                ((struct ShopFunds *)gGameState)->funds -= value;
                break;
            }
        }
        ((struct ShopPurchaseFlags *)p)->purchased = 1;
        Sound_PlaySong(0x111);
        Shop_BeginAlternateSelection(p);
    }
    return 0;
}
