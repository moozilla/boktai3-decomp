#include "global.h"

extern u8 *gUnk_02000710;

u32 sub_0803FE68(u8 *p) {
    u8 *s = p + 0x90;
    u8 *d = p + 0x68;
    u16 v;
    *(u16 *)(d + 0xe) = *(u16 *)(s + 0xe);
    *(u16 *)(d + 0x10) = *(u16 *)(s + 0x10);
    *(u16 *)(d + 0x12) = *(u16 *)(s + 0x12);
    v = *(u16 *)(s + 0x20);
    *(u16 *)(d + 0x20) = v;
    *(u16 *)(gUnk_02000710 + 0x7A2) = v;
    return 0;
}
