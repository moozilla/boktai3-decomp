#include "global.h"
s8 sub_0816B3C0(u8 *p) {
    u32 i = (p[1] << 2) + p[2];
    volatile u8 *q = p;
    q += 0xC;
    q += i;
    return (s8)*q;
}
