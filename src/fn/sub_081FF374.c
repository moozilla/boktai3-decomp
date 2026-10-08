#include "global.h"

void sub_081FF374(u8 *p, u32 a, u16 b) {
    *(u32 *)(p + 0x2C) = a;
    *(u16 *)(p + 0x30) = b;
}
