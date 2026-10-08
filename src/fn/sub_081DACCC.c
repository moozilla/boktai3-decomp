#include "global.h"

void sub_081DACCC(u8 *p, u32 a, u32 b, u32 c, u32 d, u32 e) {
    *(u32 *)(p + 0x14) = a;
    *(u32 *)(p + 0x18) = b;
    *(u32 *)(p + 0x1C) = c;
    *(u32 *)(p + 0x20) = d;
    *(u32 *)(p + 0x24) = e;
}
