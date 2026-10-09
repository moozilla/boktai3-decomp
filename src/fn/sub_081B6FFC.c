#include "global.h"

void sub_081B69F4(u8 *, u8 *, s32);
void sub_0821FE6C(u8 *);

u32 sub_081B6FFC(u8 *p) {
    u8 *q = p + 0x24;
    s32 i = 0;
    do {
        if (*(u32 *)(p + 0x18) & (1 << i)) {
            sub_081B69F4(p, q, i);
        }
        sub_0821FE6C(q + 0xBC);
        i++;
        q += 0x234;
    } while (i <= 7);
    return 0;
}
