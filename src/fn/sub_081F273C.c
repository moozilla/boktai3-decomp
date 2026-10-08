#include "global.h"

void sub_081F2208(u8 *, u8 *, s32);

u32 sub_081F273C(u8 *p) {
    u8 *q = p + 0x20;
    s32 i = 0;
    do {
        sub_081F2208(p, q, i);
        i++;
        q += 0x258;
    } while (i <= 3);
    return 0;
}
