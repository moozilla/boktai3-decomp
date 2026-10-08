#include "global.h"

u32 sub_08225B38(u8 *p, u32 *q) {
    u32 y;
    u32 x;
    u32 f = 4;
    *(u32 *)(p + 4) |= f;
    y = q[1];
    x = q[0];
    *(u32 *)(p + 0x34) = x;
    *(u32 *)(p + 0x38) = y;
    return 1;
}
