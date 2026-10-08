#include "global.h"

u16 sub_08219BD8(u8 *s)
{
    u16 h = 0;
    while (*s != 0) {
        u32 a = h << 5;
        u32 b = h >> 11;
        h = a | b;
        h = h + *s;
        s++;
    }
    return h;
}
