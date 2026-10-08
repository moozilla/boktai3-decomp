#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822B358(s32);
void sub_0822B3A4(void)
{
    if (Script_SeekToKeyword(0x69))
        sub_0822B358(Script_GetValue());
}
