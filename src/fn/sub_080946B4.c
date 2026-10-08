#include "global.h"

extern u8 *gUnk_02000580;
void sub_0812F12C(u8 *);

u32 sub_080946B4(u8 *p) {
    *(u8 **)(p + 0xdc) = gUnk_02000580 + 0x24;
    sub_0812F12C(p);
    return 0;
}
