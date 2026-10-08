#include "global.h"

void sub_08002F2C(u8 *p) {
    u32 *q = (u32 *)(p + 0x38);
    q[1] = 0x40;
    q[2] = 0x40;
    *(u32 *)(p + 0x38) = 0x40;
    q[3] = 0;
    q[4] = 0;
}
