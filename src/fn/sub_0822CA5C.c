// COMPILER: old_agbcc
#include "global.h"
extern u8 *gUnk_02000710;
extern u16 gUnk_030054C0, gUnk_030054C4;
void sub_0822C824(void);
void sub_0822CA5C(u8 *p)
{
    p[0x18] = 1;
    *(void (**)(void))(p + 0x2c) = sub_0822C824;
    {
        u32 zero = 0;
        p[0x19] = zero;
        *(u16 *)(p + 0x20) = zero;
        *(u16 *)(p + 0x22) = zero;
    }
    *(u16 *)(gUnk_02000710 + 0x874) = gUnk_030054C0;
    *(u16 *)(gUnk_02000710 + 0x876) = gUnk_030054C4;
}
