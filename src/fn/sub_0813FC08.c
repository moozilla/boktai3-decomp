#include "global.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

void sub_0813FC08(u8 *p)
{
    s32 v;

    v = Script_SeekToKeyword(0x52);
    if (v != 0) {
        *(s32 *)(p + 0xB3C) = Script_GetValue();
    } else {
        *(s32 *)(p + 0xB3C) = v;
    }
}
