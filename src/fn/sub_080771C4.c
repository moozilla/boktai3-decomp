#include "global.h"

void sub_080771C4(u8 *p) {
    u16 *q = (u16 *)(p + 0x134);
    u16 z = 0;
    q[0x1f] = z;
    *(u16 *)(p + 0x2e4) = z;
    *(u16 *)(p + 0x2e6) = z;
}
