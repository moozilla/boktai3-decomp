#include "global.h"
void sub_08020D68(void *, s32);
void sub_0802FDB8(void *unused, u8 *p) {
    u32 one;
    u32 zero;
    if (p[8] != 0) {
        p[8] = 0;
        p[7] = 0;
        p[5] = 3;
    }
    if (p[7] != 0) {
        one = 1;
        zero = 0;
        p[4] = one;
        *(u32 *)(p + 0x14) = zero;
        p[8] = one;
        sub_08020D68(p + 0x168, 1);
    }
    *(s32 *)(p + 0x14) += 1;
}
