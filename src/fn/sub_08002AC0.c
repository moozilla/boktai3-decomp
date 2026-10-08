#include "global.h"

u32 sub_08002AC0(u8 *p, u32 a, u32 b) {
    u32 off = (b << 2) + (a << 4);
    u8 *q = p + 0x25C;
    return *(u32 *)(q + off);
}
