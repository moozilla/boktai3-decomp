#include "global.h"

void sub_08077288(u8 *p) {
    u32 *a = (u32 *)(p + 0x2cc);
    u32 m = -4;
    *a = *a & m;
    if (p[8] == 0) {
        u32 *b = *(u32 **)(p + 4);
        *b = *b & (m + 2);
    } else {
        u32 *b = (u32 *)(*(u8 **)(p + 4) + 0x20);
        u32 n = -2; b[2] = b[2] & n;
    }
}
