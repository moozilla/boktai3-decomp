#include "global.h"
extern u8 *gUnk_02000710;
void Inventory_ClearEquipmentEntry(u32);
void Inventory_RemoveEquipmentEntry(s32 a)
{
    s32 i;
    Inventory_ClearEquipmentEntry(a);
    i = 0;
    do {
        u8 *g = gUnk_02000710;
        s16 *q = (s16 *)(i * 2 + (u32)g + 0x68);
        if (*q == a)
            *q = -1;
        i++;
    } while (i <= 7);
}
