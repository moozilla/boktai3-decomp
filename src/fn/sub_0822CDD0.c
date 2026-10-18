#include "global.h"
extern u8 *gUnk_02000710;
s32 Inventory_GetKeyItemId(s32 i)
{
    return *(s16 *)(gUnk_02000710 + i * 2 + 0x838);
}
