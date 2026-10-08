#include "global.h"

extern u8 *gUnk_02000710;
s32 sub_080508B4(void);

void sub_08071CB0(u8 *p) {
    if ((*(u16 *)(p + 0x2a) & 0xf) == 0) {
        if (sub_080508B4() == 0) {
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
