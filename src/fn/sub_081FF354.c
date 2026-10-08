#include "global.h"

void sub_081FF354(u8 *p, u16 a, u16 b, u8 c, u32 d) {
    *(u16 *)(p + 0) = a;
    *(u16 *)(p + 2) = b;
    *(u8 *)(p + 6) = c;
    *(u32 *)(p + 8) = d;
}
