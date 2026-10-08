#include "global.h"

extern u8 *gUnk_02000710;

u32 sub_0803FF20(u8 *p) {
    u8 *s = p + 0x90;
    u8 *d = p + 0x68;
    u16 v = *(u16 *)(s + 0x26);
    *(u16 *)(d + 0x26) = v;
    *(u16 *)(gUnk_02000710 + 0x81A) = v;
    return 0;
}
