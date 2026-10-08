#include "global.h"

void sub_0821BC68(u32, u8 *);

u32 sub_08225AF0(u8 *p, u32 a, u16 b, u16 c) {
    *(u32 *)(p + 0x1c) = a;
    sub_0821BC68(a, p + 0xc);
    *(u16 *)(p + 0x20) = b;
    *(u16 *)(p + 0x22) = c;
    return 1;
}
