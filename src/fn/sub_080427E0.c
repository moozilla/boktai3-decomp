#include "global.h"

u32 sub_080427E0(u8 *p) {
    u8 *q;
    u32 mode = *(u32 *)p;
    if (mode == 0) {
        return 0;
    }
    if (mode == 1) {
        return *(u16 *)(p + 0x12);
    }
    q = p + 0x2c;
    return *(u16 *)(q + 0x3a);
}
