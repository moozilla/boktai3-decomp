#include "global.h"

extern u8 *gUnk_02000710;
s32 OrbitBackground_IsEnabled(void);

void sub_08071CB0(u8 *p)
{
    if ((*(u16 *)(p + 0x2a) & 0xf) == 0) {
        if (OrbitBackground_IsEnabled() == 0) {
            u32 m = 0x20;
            u32 *a = (u32 *)(gUnk_02000710 + 0x868);
            *a |= m;
        } else {
            u32 *a = (u32 *)(gUnk_02000710 + 0x868);
            u32 m = ~0x20;
            *a &= m;
        }
    }
}
