#include "global.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

s32 Script_GetActorIndex(void)
{
    s32 r;
    if (Script_SeekToKeyword(0x69) == 0)
        r = 0;
    else
        r = Script_GetValue();
    return r;
}
