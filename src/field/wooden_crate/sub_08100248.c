#include "engine/shared/global.h"

s32 Script_GetKeywordValue(s32, s32);
u8 *WoodenCrate_FindByName(u32);

s32 WoodenCrate_GetFlagsFromScript(void)
{
    u32 name = Script_GetKeywordValue(0x6e, 0);
    if (name != 0) {
        u8 *p = WoodenCrate_FindByName(name);
        if (p != 0)
            return *(u16 *)(p + 0xc8);
    }
    return -1;
}
