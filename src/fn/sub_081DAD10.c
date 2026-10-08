#include "global.h"

void sub_081DAD10(u8 *p, u32 a, u32 b) {
    *(u32 *)(p + 0x48) = a;
    *(u32 *)(p + 0x4C) = b;
    if (b > 6) {
        *(u32 *)(p + 0x4C) = 0;
    }
}
