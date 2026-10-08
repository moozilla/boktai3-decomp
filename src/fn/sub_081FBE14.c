#include "global.h"

void sub_081FBE14(u8 *p, u16 a, u32 b, u8 c, u32 d, u32 e) {
    *(u16 *)(p + 2) = a;
    *(u32 *)(p + 8) = b;
    *(u8 *)(p + 6) = c;
    *(u32 *)(p + 0x14) = d;
    *(u32 *)(p + 0xC) = e;
}
