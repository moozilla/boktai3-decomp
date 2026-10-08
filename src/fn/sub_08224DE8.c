#include "global.h"

void sub_08224DE8(u8 *p) {
    u32 z = 0;
    CpuSet(&z, p, 0x05000081);
    p[0] = 1;
}
