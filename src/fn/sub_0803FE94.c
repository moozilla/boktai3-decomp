#include "global.h"

extern u8 *gUnk_02000710;

u32 sub_0803FE94(u8 *p) {
    u8 *s = p + 0x90;
    u8 *d = p + 0x68;
    u16 v;
    *(u16 *)(d + 0x14) = *(u16 *)(s + 0x14);
    *(u16 *)(d + 0x16) = *(u16 *)(s + 0x16);
    v = *(u16 *)(s + 0x22);
    *(u16 *)(d + 0x22) = v;
    *(u16 *)(gUnk_02000710 + 0x7A6) = v;
    return 0;
}
