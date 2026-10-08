#include "global.h"

void sub_08223614(void) {
    u16 z;
    u8 *p;
    u16 *q = &z;
    *q = 0;
    p = (u8 *)0x03005390;
    CpuSet(&z, p, 0x01000020);
    p[6] = 0xff;
}
