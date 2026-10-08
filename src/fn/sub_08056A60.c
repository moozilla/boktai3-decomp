#include "global.h"
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
void sub_0821AAD8(u32 *);
void sub_0821B4C8(u32 *, s32, s32);
s32 sub_08056A60(void)
{
    u32 buf[2];
    s32 i;
    sub_0821AAD8(buf);
    for (i = 0; Script_GetPc() != 0; i++)
        sub_0821B4C8(buf, i, Script_GetValue());
    return 0;
}
