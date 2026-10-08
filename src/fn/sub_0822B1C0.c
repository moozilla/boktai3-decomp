#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822B180(s32);
void sub_0822B1C0(void)
{
    if (Script_SeekToKeyword(0x66))
        sub_0822B180(Script_GetValue());
}
