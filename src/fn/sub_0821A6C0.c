#include "global.h"
u8 *sub_0821A6C0(u8 *p, u32 *out)
{
    if (p[0] & 0x80) {
        *out = ((p[0] << 8) | p[1]) & 0x7FFF;
        return p + 2;
    }
    *out = p[0];
    return p + 1;
}
