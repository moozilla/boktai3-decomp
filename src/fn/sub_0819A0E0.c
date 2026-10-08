#include "global.h"

void sub_0824923C(u8 *, u32);

void sub_0819A0E0(u8 *p) {
    if (*(p + 0xB7)) {
        u32 z = 0;
        u32 q;
        *(u32 *)p = z;
        q = *(u32 *)(p + 4);
        if (q) {
            sub_0824923C(p, q);
            *(u32 *)(p + 4) = z;
        }
    }
}
