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
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
void Shop_PrepareEquipmentDescriptor(struct ShopTail *, struct ShopPurchaseDescriptor *);
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
                s32 index = p->grid.savedSelection - 16;
                struct SceneValueEquipmentDefinition *definition =
                    Equipment_GetDefinition(catalog->catalog[index + catalog->page * 8]);
                descriptor = *(struct ShopPurchaseDescriptor *)definition;
                descriptor.modifierA = category;
                descriptor.modifierB = category;
                descriptor.modifierC = category;
                Shop_PrepareEquipmentDescriptor(p, &descriptor);
                Inventory_SetEquipmentEntry(destination, &descriptor);
                Inventory_GetEquipmentEntry(destination)->condition = 500;
                gGameState->unk7B0 -= definition->baseValue;
                break;
            }
            case 1: {
                u32 destination = p->grid.selection;
                s32 index = p->grid.savedSelection - 16;
                gGameState->unk7B0 -=
                    SceneItemValue_GetItemBaseValue(catalog->catalog[index + catalog->page * 8]);
                Inventory_SetItemId(destination, catalog->catalog[index + catalog->page * 8]);
                break;
            }
            case 2: {
                u32 destination = p->grid.selection;
                s32 index = p->grid.savedSelection - 16;
                GameState_GetConsumableDescriptor(catalog->catalog[index + catalog->page * 8]);
                Inventory_SetConsumableEntry(destination,
                                             catalog->catalog[index + catalog->page * 8]);
                gGameState->unk7B0 -= SceneItemValue_GetConsumableBaseValue(
                    catalog->catalog[index + catalog->page * 8]);
                break;
            }
        }
        p->purchased = 1;
        Sound_PlaySong(0x111);
        Shop_BeginAlternateSelection(p);
    }
    return 0;
}
