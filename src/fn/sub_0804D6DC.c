#include "global.h"
extern u8 *gUnk_02000710;
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void sub_08178238(u32, u32, u32);
u32 sub_0804D6DC(u8 *p)
{
    s32 *q = (s32 *)(gUnk_02000710 + 0x618);
    s32 r;
    if (*q <= 0) *q = 1;
    if (Script_SeekToKeyword(0x73)) r = Script_GetValue();
    else r = 0x1E;
    *(u16 *)(p + 0x1A) = r;
    *(u16 *)(p + 0x18) = 0;
    sub_08178238(*(u32 *)(gUnk_02000710 + 0x618), *(u32 *)(gUnk_02000710 + 0x61C), 0);
    return 0;
}
