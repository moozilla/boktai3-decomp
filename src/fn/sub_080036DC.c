#include "global.h"

extern u8 *gUnk_02000468;

u32 sub_080036DC(void) {
    u8 *q = gUnk_02000468;
    u32 r;
    if (q) {
        q += 0x4C;
        r = q[1];
    } else {
        r = 0;
    }
    return r;
}
