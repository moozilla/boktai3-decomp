#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822B058(s32);
void sub_0822B0B0(void)
{
    if (Script_SeekToKeyword(0x66))
        sub_0822B058(Script_GetValue());
}
