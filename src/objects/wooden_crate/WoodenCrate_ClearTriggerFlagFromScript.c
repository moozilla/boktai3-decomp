#include "global.h"

s32 Script_GetKeywordValue(s32, s32);
s32 WoodenCrate_FindByName(u32);
void WoodenCrate_ClearFlags(s32, s32);

void WoodenCrate_ClearTriggerFlagFromScript(void)
{
    u32 name = Script_GetKeywordValue(0x6e, 0);
    if (name != 0) {
        s32 r = WoodenCrate_FindByName(name);
        if (r != 0)
            WoodenCrate_ClearFlags(r, 0x10);
    }
}
