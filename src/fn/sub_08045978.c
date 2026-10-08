#include "global.h"

void sub_08045978(u8 *p, u32 a, u32 b, u32 c) {
    *(u32 *)(p + 0x38) = a;
    *(u32 *)(p + 0x3c) = b;
    *(u32 *)(p + 0x40) = c;
}
