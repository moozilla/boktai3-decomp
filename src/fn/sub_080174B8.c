#include "global.h"

void sub_080174B8(u8 *p, u32 v) {
    u32 z = 0;
    p[0x1D] = v;
    *(u32 *)(p + 0x18) = z;
    p[0x1C] = 1;
}
