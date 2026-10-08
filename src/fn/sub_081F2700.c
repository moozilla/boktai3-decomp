#include "global.h"

void sub_081F23A0(u8 *, u8 *, s32);
void sub_0811B394(u8 *);

u32 sub_081F2700(u8 *p) {
    u8 *q = p + 0x20;
    s32 i = 0;
    do {
        if (*(u32 *)(p + 0x18) & (1 << i)) {
            sub_081F23A0(p, q, i);
        }
        sub_0811B394(q + 0xC8);
        i++;
        q += 0x258;
    } while (i <= 3);
    return 0;
}
