#include "global.h"

void sub_08008A98(u8 *p, u32 v) {
    u32 z = 0;
    p[2] = v;
    *(u32 *)(p + 0xC) = z;
    p[4] = 1;
}
