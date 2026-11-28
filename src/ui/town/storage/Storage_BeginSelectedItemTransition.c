#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_BeginItemSpecialMove(struct StorageScreen *);
void Storage_BeginEquipmentSpecialMove(struct StorageScreen *);
void sub_081BE730(struct StorageScreen *);
void Storage_BeginSelectedItemTransition(struct StorageScreen *screen)
{
    switch (screen->categoryKinds[screen->category] >> 1) {
        case 0:
            Storage_BeginItemSpecialMove(screen);
            break;
        case 1:
            Storage_BeginEquipmentSpecialMove(screen);
            break;
        case 2:
            sub_081BE730(screen);
            break;
    }
}
