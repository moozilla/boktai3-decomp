#include "global.h"

void sub_081DACE4(u8 *p, u16 a, u16 b, u16 c) {
    *(u16 *)(p + 0x28) = a;
    *(u16 *)(p + 0x2A) = b;
    *(u16 *)(p + 0x2C) = c;
}
