#include "global.h"

void sub_0824923C(u8 *, u32);

u32 sub_0803F690(u8 *p) {
    u8 *n = *(u8 **)(p + 0x1c);
    while (n != 0) {
        sub_0824923C(n, *(u32 *)(n + 0x34));
        n = *(u8 **)(n + 0x3c);
    }
    return 0;
}
