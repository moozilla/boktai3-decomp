#include "global.h"

void sub_0803F804(u8 *p, u32 n) {
    *(u32 *)(p + 0x18) |= 1 << n;
    *(u16 *)(p + 0xa) += 1;
}
