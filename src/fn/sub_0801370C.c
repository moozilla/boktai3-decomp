#include "global.h"

void sub_0824923C(u8 *p);
void sub_08013740(void);

void sub_0801370C(u8 *p) {
    *(u32 *)(p + 0xC) &= ~1;
    *(u32 *)(p + 0x60) = (u32)sub_08013740;
    sub_0824923C(p);
}
