#include "global.h"

s32 sub_0803F5C4(u8 *p, u8 *q) {
    u32 v = *q;
    if (v != 0) {
        return -1;
    }
    *(u32 *)(q + 0x38) = v;
    v = *(u32 *)(p + 0x1c);
    *(u32 *)(q + 0x3c) = v;
    if (v != 0) {
        *(u32 *)(v + 0x38) = (u32)q;
    }
    *(u32 *)(p + 0x1c) = (u32)q;
    *q = 1;
    return 0;
}
