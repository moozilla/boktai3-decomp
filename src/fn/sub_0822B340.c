#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822B330(s32);
void sub_0822B340(void)
{
    if (Script_SeekToKeyword(0x69))
        sub_0822B330(Script_GetValue());
}
