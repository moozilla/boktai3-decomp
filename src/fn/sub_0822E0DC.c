#include "global.h"
extern u8 *gUnk_02000710;
void Inventory_SetFlag(u32);
void Inventory_SetConsumableEntry(u32 i, u32 v)
{
    Inventory_SetFlag(v);
    *(u16 *)(gUnk_02000710 + i * 2 + 0x160) = v;
}
