#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822E498(s32);
void sub_0822E504(void)
{
    if (Script_SeekToKeyword(0x6D))
        sub_0822E498(Script_GetValue());
}
