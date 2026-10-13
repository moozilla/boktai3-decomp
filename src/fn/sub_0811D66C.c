#include "global.h"
u32 Script_GetKeywordValue(u32, s32);
void sub_0811D5A8(u32);
void sub_0811D66C(void)
{
    s32 m = -1;
    u32 a = Script_GetKeywordValue(0x63, m);
    u32 b = Script_GetKeywordValue(0x74, m);
    if (a <= 11 && b <= 60000)
        sub_0811D5A8(a);
}
