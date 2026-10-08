#include "global.h"

void sub_081F21F8(u8 *p) {
    u8 one = 1;
    u32 z = 0;
    *(u8 *)(p + 1) = one;
    *(u8 *)(p + 0) = one;
    *(u32 *)(p + 0x84) = z;
}
