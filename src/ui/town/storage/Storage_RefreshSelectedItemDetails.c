#include "global.h"
void Storage_ShowItemDescription(void *);
void Storage_ShowEquipmentDescription(void *);
void Storage_ShowConsumableDescription(void *);
void Storage_RefreshSelectedItemDetails(u8 *p)
{
    u8 *a = p + 0x1754, *b = p + 0x1758;
    switch (b[*a] >> 1) {
        case 0:
            Storage_ShowItemDescription(p);
            break;
        case 1:
            Storage_ShowEquipmentDescription(p);
            break;
        case 2:
            Storage_ShowConsumableDescription(p);
            break;
    }
}
