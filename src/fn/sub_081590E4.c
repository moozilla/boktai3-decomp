#include "global.h"

void Inventory_InitEquipmentEntryFromDescriptor(s32, s32 *);
s32 sub_08159030(s32 *, s32);

s32 Player_GetEquipmentValue(s32 a)
{
    s32 buf[4];

    Inventory_InitEquipmentEntryFromDescriptor(a, buf);
    return sub_08159030(buf, 0);
}
