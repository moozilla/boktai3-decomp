#include "global.h"

void sub_081FE970(u8 *p, u16 a, u32 b, u8 c, u8 d) {
    *(u16 *)(p + 2) = a;
    *(u32 *)(p + 0x18) = b;
    *(u8 *)(p + 6) = c;
    *(u8 *)(p + 0xC) = d;
}
