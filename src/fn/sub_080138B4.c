#include "global.h"

void sub_0824923C(u8 *p, u32 f);

u32 sub_080138B4(u8 *p) {
    u8 *n = *(u8 **)(p + 0x1C);
    if (n) {
        do {
            sub_0824923C(n, *(u32 *)(n + 0x60));
            n = *(u8 **)(n + 0x68);
        } while (n);
    }
    return 0;
}
