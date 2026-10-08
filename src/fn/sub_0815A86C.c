#include "global.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
void sub_0815A86C(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p) {
        u16 *a = (u16 *)(p + 0x534);
        u32 z;
        z = 0;
        *a = z;
        *(u16 *)(p + 0x538) = z;
        *(u16 *)(p + 0x536) = z;
    }
}
