#include "global.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

void sub_0813FBC8(u8 *p)
{
    s32 v;

    v = Script_SeekToKeyword(0x69);
    if (v != 0) {
        v = Script_GetValue();
    }
    *(s32 *)(p + 0x18) = v;
}
