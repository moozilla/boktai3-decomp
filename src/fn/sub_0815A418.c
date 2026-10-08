#include "global.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

s32 sub_0815A418(s16 *p)
{
    s32 r;
    if (Script_SeekToKeyword(0x70) == 0) {
        r = 0;
    } else {
        p[0] = Script_GetValue();
        p[1] = Script_GetValue();
        p[2] = Script_GetValue();
        r = 1;
    }
    return r;
}
