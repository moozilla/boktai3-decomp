#include "global.h"

void sub_0819A9DC(u8 *p, u8 v) {
    u32 z;
    u8 *q = p + 0x3A;
    z = 0;
    *q = v;
    *(p + 0x3B) = z;
    *(u16 *)(p + 0x38) = z;
}
