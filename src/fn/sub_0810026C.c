#include "global.h"

s32 Script_GetKeywordValue(s32, s32);
u8 *WoodenCrate_FindByName(void);
void WoodenCrate_SetFlags(u8 *, s32);
void WoodenCrate_ClearFlags(u8 *, s32);

void WoodenCrate_StartBreakFromScript(void)
{
    if (Script_GetKeywordValue(0x6e, 0) != 0) {
        u8 *p = WoodenCrate_FindByName();
        if (p != 0) {
            WoodenCrate_SetFlags(p, 0x18);
            WoodenCrate_ClearFlags(p, 4);
        }
    }
}
