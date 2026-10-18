#include "global.h"
void Inventory_SortConsumables(u32);
void sub_0822DA94(u32);
void GameState_SortConsumableSlots(u32);
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x1754];
    u8 selectedCategory;
    u8 unknown1755[3];
    u8 categories[0x48];
    u8 sortMode;
};
void SceneInventory_SortSelectedCategory(struct SceneInventoryCategoryMenu *p)
{
    switch (p->categories[p->selectedCategory] >> 1) {
        case 0:
            Inventory_SortConsumables(p->sortMode);
            break;
        case 1:
            sub_0822DA94(p->sortMode);
            break;
        case 2:
            GameState_SortConsumableSlots(p->sortMode);
            break;
    }
}
