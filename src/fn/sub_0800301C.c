#include "global.h"

void sub_0800301C(u8 *p) {
    u32 *q = (u32 *)(p + 0x25C);
    s32 i = 3;
    do {
        q[8] = *q;
        q++;
        i--;
    } while (i >= 0);
}
