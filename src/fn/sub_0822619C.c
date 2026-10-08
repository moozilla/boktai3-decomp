#include "global.h"

void sub_0822619C(u32 *q) {
    u8 *p = *(u8 **)0x03005420;
    if (p != 0) {
        u32 a = q[0];
        u32 b = q[1];
        *(u32 *)(p + 0x74) = a;
        *(u32 *)(p + 0x78) = b;
    }
}
