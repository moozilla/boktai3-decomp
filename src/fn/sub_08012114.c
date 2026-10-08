#include "global.h"

void sub_0801204C(u32 unused, u8 *p, u32 v);

void sub_08012114(u32 a, u8 *p) {
    *(u32 *)(p + 0x64) |= 1;
    sub_0801204C(a, p, 0);
    p[0] = 0;
}
