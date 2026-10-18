#include "global.h"

u32 Script_GetKeywordValue(u32, u32);
u32 ShadowRegistry_SetEntryMode(u32, u32);
u32 ShadowRegistry_SetModeFromScript(void)
{
    u32 a = Script_GetKeywordValue(0x6e, 0);
    u32 b = Script_GetKeywordValue(0x6d, 1);
    return ShadowRegistry_SetEntryMode(a, b);
}
