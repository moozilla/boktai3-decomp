#include "global.h"

void sub_08202F1C(u8 *p, u32 a, u32 b, u32 c) {
    *(u32 *)(p + 0x34) = a;
    *(u32 *)(p + 0x38) = b;
    *(u32 *)(p + 0x3C) = c;
}
