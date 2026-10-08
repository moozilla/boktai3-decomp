#include "global.h"

extern u8 *gUnk_020004A8;

u16 sub_08072814(void) {
    u16 r;
    if (gUnk_020004A8) r = *(u16 *)(gUnk_020004A8 + 0x62);
    else r = 0;
    return r;
}
