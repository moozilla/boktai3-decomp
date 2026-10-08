#include "global.h"
extern u8 *gUnk_02000580[];
extern u8 *gUnk_02000710;
s32 sub_0815A400(void);
u32 sub_0821ABA8(u32, u32);
void sub_0815A628(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p) {
        u32 r = sub_0821ABA8(0x65, *(u16 *)(p + 0x42a));
        *(u16 *)(p + 0x428) = r;
        *(u16 *)(gUnk_02000710 + 0x2c) = r;
    }
}
