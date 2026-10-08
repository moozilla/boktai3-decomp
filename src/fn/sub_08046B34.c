#include "global.h"

void sub_08046B34(u8 *p, u32 n) {
    *(u32 *)(p + 0x18) |= 1 << n;
}
