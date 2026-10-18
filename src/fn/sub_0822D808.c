#include "global.h"
extern u8 *gUnk_02000710;
void ItemDescriptor_Reset(void *);
void Inventory_GetEquipmentDescriptor(s32, void *);
void Inventory_GetEquippedDescriptor(u32 i, void *p)
{
    u8 *g = gUnk_02000710;
    u8 *b = (u8 *)(i * 2 + (u32)g);
    s16 *q = (s16 *)(b + 0x68);
    if (*q < 0)
        ItemDescriptor_Reset(p);
    else
        Inventory_GetEquipmentDescriptor(*q, p);
}
