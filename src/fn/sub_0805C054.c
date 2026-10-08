#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0805C054(u8 *p)
{
    s32 v = Script_SeekToKeyword(0x70);
    if (v) *(s32 *)(p + 0x17ac) = Script_GetValue();
    else *(s32 *)(p + 0x17ac) = v;
}
