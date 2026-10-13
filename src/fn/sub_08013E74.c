#include "global.h"

void Script_GetKeywordValue(u32, u32);
u32 ShadowRegistry_Remove(void);
u32 sub_08013E74(void)
{
    Script_GetKeywordValue(0x6e, 0);
    return ShadowRegistry_Remove();
}
