#include "global.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
void sub_0815AAFC(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p)
        *(u16 *)(p + 0x4a2) = 1;
}
