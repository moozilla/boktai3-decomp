// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_tail.h"
#include "scene_inventory_catalog.h"
#include "scene_item_values.h"
struct SceneInventoryPurchaseDescriptor {
    u32 prefix[3];
    u8 modifierA, unknownD[3], modifierB, unknown11[3], modifierC, unknown15[11];
};
struct SceneInventorySwapView {
    u8 unknown0[0x1AF0];
    struct SceneInputPair pair;
};
struct SceneInventoryPurchaseFlags {
    u8 unknown0[0x1AAC];
    u8 purchased;
};
struct SceneInventoryFunds {
    u8 unknown0[0x7B0];
    s16 funds;
};
s32 SceneInput_TickPairTransition(struct SceneInputPair *);
void sub_0805F900(struct SceneInventoryTail *, struct SceneInventoryPurchaseDescriptor *);
void sub_0822D83C(u32, struct SceneInventoryPurchaseDescriptor *);
void sub_0822CDEC(u32, s32);
void sub_0822E0DC(u32, s32);
s32 SceneInventory_TickPurchaseTransition(struct SceneInventoryTail *p)
{
    struct SceneInventoryPurchaseDescriptor descriptor;
    struct SceneInventoryScriptCatalog *catalog = (struct SceneInventoryScriptCatalog *)p;
    s32 category;
    SceneInputPanel_ResetHighlights(&p->display, &p->highlight);
    if (SceneInput_TickPairTransition(&((struct SceneInventorySwapView *)p)->pair)) {
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
                    definition = sub_08227F5C(offset);
                }
                descriptor = *(struct SceneInventoryPurchaseDescriptor *)definition;
                descriptor.modifierA = category;
                descriptor.modifierB = category;
                descriptor.modifierC = category;
                sub_0805F900(p, &descriptor);
                sub_0822D83C(destination, &descriptor);
                sub_0822D6C8(destination)->condition = 500;
                ((struct SceneInventoryFunds *)gUnk_02000710)->funds -= definition->baseValue;
                break;
            }
            case 1: {
                u32 destination = p->grid.selection;
                s32 index = p->grid.savedSelection - 16;
                u8 *page = &catalog->page;
                s8 *entries;
                register s32 offset asm("r0") = *page * 8;
                s32 value;
                asm("" : "+r"(offset), "+r"(index));
                offset = index + offset;
                entries = catalog->catalog;
                value = SceneItemValue_GetItemBaseValue(entries[offset]);
                ((struct SceneInventoryFunds *)gUnk_02000710)->funds -= value;
                index += *page * 8;
                entries += index;
                sub_0822CDEC(destination, *entries);
                break;
            }
            case 2: {
                u32 destination = p->grid.selection;
                s32 index = p->grid.savedSelection - 16;
                u8 *page = &catalog->page;
                s8 *entries;
                register s32 offset asm("r0") = *page * 8;
                s32 value;
                asm("" : "+r"(offset), "+r"(index));
                offset = index + offset;
                entries = catalog->catalog;
                sub_0822E288(entries[offset]);
                offset = *page * 8;
                offset = index + offset;
                sub_0822E0DC(destination, entries[offset]);
                index += *page * 8;
                entries += index;
                value = SceneItemValue_GetConsumableBaseValue(*entries);
                ((struct SceneInventoryFunds *)gUnk_02000710)->funds -= value;
                break;
            }
        }
        ((struct SceneInventoryPurchaseFlags *)p)->purchased = 1;
        Sound_PlaySong(0x111);
        SceneInventory_BeginAlternateSelection(p);
    }
    return 0;
}
