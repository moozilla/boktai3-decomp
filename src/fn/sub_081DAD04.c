#include "global.h"

void sub_081DAD04(u8 *p, u32 a, u32 b, u32 c) {
    *(u32 *)(p + 0x3C) = a;
    *(u32 *)(p + 0x40) = b;
    *(u32 *)(p + 0x44) = c;
}
