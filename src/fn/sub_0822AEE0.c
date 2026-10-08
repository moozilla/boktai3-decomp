#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822AE7C(s32);
void sub_0822AEE0(void)
{
    if (Script_SeekToKeyword(0x69))
        sub_0822AE7C(Script_GetValue());
}
