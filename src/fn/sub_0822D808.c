#include "global.h"
extern u8 *gUnk_02000710;
void sub_0822D7C0(void *);
void Inventory_GetEquipmentDescriptor(s32, void *);
void Inventory_GetEquippedDescriptor(u32 i, void *p)
{
    u8 *g = gUnk_02000710;
    u8 *b = (u8 *)(i * 2 + (u32)g);
    s16 *q = (s16 *)(b + 0x68);
    if (*q < 0)
        sub_0822D7C0(p);
    else
        Inventory_GetEquipmentDescriptor(*q, p);
}
