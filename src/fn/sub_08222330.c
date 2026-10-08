#include "global.h"

void sub_082221F4(u8 *, u32, u8 *, u16 *);

void sub_08222330(u8 *p, u32 q) {
    u8 a;
    u16 b;
    sub_082221F4(p, q, &a, &b);
    *(u32 *)(p + 4) = a << 8;
    *(u32 *)(p + 0x10) = b << 8;
}
