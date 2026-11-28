#include "global.h"
#include "ui/town/storage_screen.h"
void sub_0822D080(u32, u32);
void Inventory_SwapEquipmentEntries(u32, u32);
void MenuPanel_SwapConsumableSlots(u32, u32);
void sub_081BECF0(struct StorageScreen *screen)
{
    switch (screen->categoryKinds[screen->category] >> 1) {
        case 0:
            sub_0822D080(screen->slots[screen->selection], screen->slots[screen->savedSelection]);
            break;
        case 1:
            Inventory_SwapEquipmentEntries(screen->slots[screen->selection],
                                           screen->slots[screen->savedSelection]);
            break;
        case 2:
            MenuPanel_SwapConsumableSlots(screen->slots[screen->selection],
                                          screen->slots[screen->savedSelection]);
            break;
    }
}
