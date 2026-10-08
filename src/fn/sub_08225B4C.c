#include "global.h"

void sub_0821E5C4(u32, u8, u8, u8);

u32 sub_08225B4C(u8 *p, u32 a, u8 b, u8 c, u8 d) {
    *(u32 *)(p + 0x28) = a;
    sub_0821E5C4(a, b, c, d);
    return 1;
}
