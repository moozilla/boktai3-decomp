#include "engine/shared/global.h"

s32 Script_GetKeywordValue(s32, s32);
u8 *WoodenCrate_FindByName(u32);
void WoodenCrate_SetFlags(u8 *, s32);
void WoodenCrate_ClearFlags(u8 *, s32);

void WoodenCrate_StartBreakFromScript(void)
{
    u32 name = Script_GetKeywordValue(0x6e, 0);
    if (name != 0) {
        u8 *p = WoodenCrate_FindByName(name);
        if (p != 0) {
            WoodenCrate_SetFlags(p, 0x18);
            WoodenCrate_ClearFlags(p, 4);
        }
    }
}
