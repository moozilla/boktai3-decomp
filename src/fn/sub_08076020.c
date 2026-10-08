#include "global.h"

void sub_08076020(u8 *p, u32 a, u8 b, u32 c) {
    *(u32 *)(p + 0x3d0) = a;
    *(u32 *)p = c;
    p[8] = b;
}
