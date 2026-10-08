#include "global.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

s32 sub_0815C9EC(void)
{
    s32 r;
    if (Script_SeekToKeyword(0x64) == 0)
        r = -1;
    else
        r = Script_GetValue();
    return r;
}
