#include "global.h"

void sub_08045934(u8 *p, u16 a, u16 b, u8 c, u32 d) {
    *(u16 *)p = a;
    *(u16 *)(p + 2) = b;
    *(u8 *)(p + 6) = c;
    *(u32 *)(p + 8) = d;
}
