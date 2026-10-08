#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822AF10(s32);
void sub_0822AF5C(void)
{
    s32 v;
    if (Script_SeekToKeyword(0x69))
        v = Script_GetValue();
    else
        v = 0;
    sub_0822AF10(v);
}
