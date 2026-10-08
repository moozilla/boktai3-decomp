#include "global.h"

void sub_081959C0(u8 *p) {
    s32 i = 0;
    u8 *a = p + 0x726A;
    u32 z = 0;
    u8 *b = p + 0x7255;
    u16 *d = (u16 *)(p + 0x7278);
    do {
        *d = *(u16 *)a;
        b[i] = z;
        d++;
        i++;
    } while (i <= 8);
}
