#include "global.h"

void sub_08045948(u8 *p, u16 a, u32 b, u8 c) {
    *(u16 *)(p + 4) = a;
    *(u32 *)(p + 0xc) = b;
    *(u8 *)(p + 7) = c;
}
