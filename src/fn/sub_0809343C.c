#include "global.h"

void sub_0809343C(u8 *p) {
    u8 *q = *(u8 **)(p + 0x3d0);
    u8 v = q[0x3dc];
    if (v != 0) q[0x3dc] = v - 1;
}
