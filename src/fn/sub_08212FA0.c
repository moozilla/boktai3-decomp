#include "global.h"
s32 Script_SeekToKeyword(s32);
u32 Script_GetValue(void);
s32 sub_08212F50(u32);
s32 sub_08212FA0(void)
{
    s32 r;
    if (!Script_SeekToKeyword(0x69))
        return -1;
    r = sub_08212F50(Script_GetValue());
    if (r > 0)
        r = 1;
    return r;
}
