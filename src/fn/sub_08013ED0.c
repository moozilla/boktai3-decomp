#include "global.h"

u32 Script_GetKeywordValue(u32, u32);
u32 sub_08013E88(u32, u32);
u32 sub_08013ED0(void)
{
    u32 a = Script_GetKeywordValue(0x6e, 0);
    u32 b = Script_GetKeywordValue(0x6d, 1);
    return sub_08013E88(a, b);
}
