#include "global.h"

extern u8 *gUnk_02000710;

u32 sub_0803FD8C(u8 *p) {
    u8 *s = p + 0x90;
    u8 *d = p + 0x68;
    u16 v;
    *(u16 *)(d + 4) = *(u16 *)(s + 4);
    *(u16 *)(d + 6) = *(u16 *)(s + 6);
    *(u16 *)(d + 8) = *(u16 *)(s + 8);
    *(u16 *)(d + 0xc) = *(u16 *)(s + 0xc);
    v = *(u16 *)(s + 0x1e);
    *(u16 *)(d + 0x1e) = v;
    *(u16 *)(gUnk_02000710 + 0x7A4) = v;
    return 0;
}
