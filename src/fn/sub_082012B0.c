#include "global.h"

void sub_082012B0(u8 *p, u16 a, u32 b, u8 c) {
    *(u16 *)(p + 2) = a;
    *(u32 *)(p + 0x8) = b;
    *(u8 *)(p + 6) = c;
}
