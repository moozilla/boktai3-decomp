#include "global.h"
extern u8 *gUnk_02000580[];
extern u8 *gUnk_02000710;
s32 sub_0815A400(void);
void sub_0815AA7C(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p) {
        u32 k = p[0x472];
        u8 *b = gUnk_02000710;
        *(u32 *)(b + k * 4 + 0x628) = 0;
    }
}
