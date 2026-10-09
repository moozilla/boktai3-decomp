#include "global.h"

void sub_08044118(u8 *, u8 *, s32);

u32 sub_080447AC(u8 *p) {
    u8 *q = p + 0x24;
    s32 i = 0;
    do {
        sub_08044118(p, q, i);
        i++;
        q += 0x258;
    } while (i <= 15);
    return 0;
}
