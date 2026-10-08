#include "global.h"

void sub_081FE984(u8 *p, u16 a, u16 b, u16 c) {
    *(u16 *)(p + 0xE) = a;
    *(u16 *)(p + 0x10) = b;
    *(u16 *)(p + 0x12) = c;
}
