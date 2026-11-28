#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_ShowItemDescription(struct StorageScreen *);
void Storage_ShowEquipmentDescription(struct StorageScreen *);
void Storage_ShowConsumableDescription(struct StorageScreen *);
void Storage_RefreshSelectedItemDetails(struct StorageScreen *screen)
{
    switch (screen->categoryKinds[screen->category] >> 1) {
        case 0:
            Storage_ShowItemDescription(screen);
            break;
        case 1:
            Storage_ShowEquipmentDescription(screen);
            break;
        case 2:
            Storage_ShowConsumableDescription(screen);
            break;
    }
}
