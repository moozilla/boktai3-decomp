#include "global.h"
s32 sub_0805B318(u8 *p)
{
    s32 i = 0;
    s32 n = p[0x1e];
    if (i < n) {
        s32 m = n;
        u8 **q = (u8 **)(p + 0xbe8);
        do {
            if ((*q)[0xd3] != 2) return 0;
            q++;
            i++;
        } while (i < m);
    }
    return 1;
}
