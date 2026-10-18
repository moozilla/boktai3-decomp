#include "global.h"
extern u8 *gUnk_02000710;
u32 Inventory_GetItemStateValue(u32 i)
{
    return *(u16 *)(gUnk_02000710 + i * 2 + 0x100) & 0x7FFF;
}
