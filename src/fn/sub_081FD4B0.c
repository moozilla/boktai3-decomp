#include "global.h"

void sub_081FD4B0(u8 *p, u32 a, u16 b) {
    *(u32 *)(p + 0x48) = a;
    *(u16 *)(p + 0x4C) = b;
}
