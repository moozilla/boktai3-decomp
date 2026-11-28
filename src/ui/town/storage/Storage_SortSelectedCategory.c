#include "global.h"
#include "ui/town/storage_screen.h"
void Inventory_SortConsumables(u32);
void Inventory_SortEquipment(u32);
void GameState_SortConsumableSlots(u32);
void Storage_SortSelectedCategory(struct StorageScreen *screen)
{
    switch (screen->categoryKinds[screen->category] >> 1) {
        case 0:
            Inventory_SortConsumables(screen->slots[16]);
            break;
        case 1:
            Inventory_SortEquipment(screen->slots[16]);
            break;
        case 2:
            GameState_SortConsumableSlots(screen->slots[16]);
            break;
    }
}
