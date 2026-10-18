#include "global.h"
extern u8 *gUnk_02000710;
u32 Inventory_GetItemOverrideFlag(u32 i)
{
    return *(s16 *)(gUnk_02000710 + i * 2 + 0x100) & 0x8000;
}
