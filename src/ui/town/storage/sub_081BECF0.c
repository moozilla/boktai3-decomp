#include "global.h"
void sub_0822D080(u32, u32);
void Inventory_SwapEquipmentEntries(u32, u32);
void MenuPanel_SwapConsumableSlots(u32, u32);
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x1754];
    u8 selectedCategory;
    u8 unknown1755[3];
    u8 categories[0xB];
    u8 swapFirst;
    u8 unknown1764[2];
    u8 swapSecond;
    u8 unknown1767[0x29];
    u8 slots[1];
};
void sub_081BECF0(struct SceneInventoryCategoryMenu *p)
{
    switch (p->categories[p->selectedCategory] >> 1) {
        case 0:
            sub_0822D080(p->slots[p->swapFirst], p->slots[p->swapSecond]);
            break;
        case 1:
            Inventory_SwapEquipmentEntries(p->slots[p->swapFirst], p->slots[p->swapSecond]);
            break;
        case 2:
            MenuPanel_SwapConsumableSlots(p->slots[p->swapFirst], p->slots[p->swapSecond]);
            break;
    }
}
