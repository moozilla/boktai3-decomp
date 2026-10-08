#include "global.h"

u32 sub_080937A4(u8 *p) {
    u32 r;
    switch (*(u16 *)(p + 0x1e6)) {
    case 0: r = 7; break;
    case 1: r = 4; break;
    case 2: r = 6; break;
    default: r = 4; break;
    }
    return r;
}
