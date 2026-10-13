#include "global.h"
u32 Script_GetKeywordValue(u32, s32);
void sub_0811D600(u32, u32);
void sub_0811D63C(void)
{
    s32 m = -1;
    u32 a = Script_GetKeywordValue(0x63, m);
    u32 b = Script_GetKeywordValue(0x72, m);
    if (a <= 11 && b <= 4)
        sub_0811D600(a, b);
}
