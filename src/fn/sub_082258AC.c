#include "global.h"

void sub_082258AC(u8 *n) {
    u8 *s = *(u8 **)0x030025F8;
    if (s != 0) {
        u32 v = *(u32 *)(s + 0x18);
        if (v == 0) {
            *(u8 **)(s + 0x18) = n;
            *(u8 **)(s + 0x1c) = n;
            *(u32 *)(n + 0x40) = v;
            *(u32 *)(n + 0x44) = v;
        } else {
            u8 *t = *(u8 **)(s + 0x1c);
            *(u8 **)(t + 0x44) = n;
            *(u8 **)(n + 0x40) = t;
            *(u32 *)(n + 0x44) = 0;
            *(u8 **)(s + 0x1c) = n;
        }
    }
}
