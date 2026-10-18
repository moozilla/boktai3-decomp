#include "global.h"

s32 Script_GetKeywordValue(s32, s32);
u8 *WoodenCrate_FindByName(void);

s32 WoodenCrate_GetFlagsFromScript(void)
{
    if (Script_GetKeywordValue(0x6e, 0) != 0) {
        u8 *p = WoodenCrate_FindByName();
        if (p != 0)
            return *(u16 *)(p + 0xc8);
    }
    return -1;
}
