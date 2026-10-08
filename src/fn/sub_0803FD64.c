#include "global.h"

extern u8 *gUnk_02000710;

u32 sub_0803FD64(u8 *p) {
    u8 *s = p + 0x90;
    u8 *d = p + 0x68;
    u16 v;
    *(u16 *)d = *(u16 *)s;
    *(u16 *)(d + 2) = *(u16 *)(s + 2);
    v = *(u16 *)(s + 0x1c);
    *(u16 *)(d + 0x1c) = v;
    *(u16 *)(gUnk_02000710 + 0x7A0) = v;
    return 0;
}
