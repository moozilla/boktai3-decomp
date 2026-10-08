#include "global.h"

void sub_08045D30(u8 *p, u32 v) {
    u32 z = 0;
    *(u8 *)(p + 1) = v;
    *(u32 *)(p + 0xc) = z;
    *(u8 *)(p + 2) = 1;
}
