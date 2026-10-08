#include "global.h"

u32 sub_08219C40(u32);
void sub_08219DD8(u32, u32);

s32 sub_082223CC(u32 *p, u32 n, u32 c) {
    u32 sz;
    s32 r;
    p[0] = n;
    sz = n << 2;
    p[1] = sub_08219C40(sz);
    if (p[1] != 0) {
        sub_08219DD8(p[1], sz);
        p[2] = c;
        r = 0;
    } else {
        r = -1;
    }
    return r;
}
