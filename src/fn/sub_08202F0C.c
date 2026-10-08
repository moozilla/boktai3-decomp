#include "global.h"

void sub_08202F0C(u8 *p, u16 a, u16 b, u16 c) {
    *(u16 *)(p + 0x24) = a;
    *(u16 *)(p + 0x26) = b;
    *(u16 *)(p + 0x28) = c;
}
