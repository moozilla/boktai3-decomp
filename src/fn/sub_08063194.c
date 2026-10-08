#include "global.h"

s32 Script_SeekToKeyword(s32);
u32 Script_GetValue(void);

void sub_08063194(u8 *p)
{
    s32 r = Script_SeekToKeyword(0x65);
    if (r)
        *(u32 *)(p + 0x1CF4) = Script_GetValue();
    else
        *(u32 *)(p + 0x1CF4) = r;
}
