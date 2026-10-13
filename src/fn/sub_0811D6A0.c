#include "global.h"
s32 Script_GetKeywordValue(u32, s32);
void sub_0811BA54(void);
void sub_0811D6A0(void)
{
    s32 m = -1;
    if (Script_GetKeywordValue(0x63, m) >= 0 && Script_GetKeywordValue(0x74, m) >= 0)
        sub_0811BA54();
}
