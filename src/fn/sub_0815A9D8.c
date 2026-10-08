#include "global.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0813D8F4(u8 *, s32);
void sub_0815A9D8(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p != 0 && Script_SeekToKeyword(0x73) != 0)
        sub_0813D8F4(p, Script_GetValue());
}
