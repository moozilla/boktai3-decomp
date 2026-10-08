#include "global.h"

void sub_0824923C(u8 *);

void sub_0803F61C(u8 *p) {
    *(u32 *)(p + 0xc) &= ~1;
    *(u32 *)(p + 0x34) = 0x0803F651;
    sub_0824923C(p);
}
