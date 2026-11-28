#include "global.h"
s32 Inventory_GetEquipmentId(s32);
void Inventory_SwapEquipmentEntries(s32, s32);
void Inventory_SortEquipment(s32 base)
{
    s32 count = 0;
    s32 gaps = 0;
    s32 i, j;

    for (i = 0; i < 16; i++) {
        if (Inventory_GetEquipmentId(base + i) == 0xFF) {
            gaps++;
        } else {
            count++;
            if (gaps > 0)
                Inventory_SwapEquipmentEntries(base + i, base + i - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++) {
            for (j = i; j < count; j++) {
                if (Inventory_GetEquipmentId(base + i) > Inventory_GetEquipmentId(base + j))
                    Inventory_SwapEquipmentEntries(base + i, base + j);
            }
        }
    }
}
