#include "global.h"

void sub_081FF37C(u8 *p, u32 a, u32 b) {
    *(u32 *)(p + 0x10) = a;
    *(u32 *)(p + 0x14) = b;
    *(u32 *)(p + 0x18) = 0;
}
