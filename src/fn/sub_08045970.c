#include "global.h"

void sub_08045970(u8 *p, u16 a, u16 b, u16 c) {
    *(u16 *)(p + 0x30) = a;
    *(u16 *)(p + 0x32) = b;
    *(u16 *)(p + 0x34) = c;
}
