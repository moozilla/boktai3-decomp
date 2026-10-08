#include "global.h"
s32 sub_0805C7B4(u8 *p)
{
    s32 n = 0;
    s32 c = p[0x1e];
    if (n < c) {
        u8 **q = (u8 **)(p + 0xbe8);
        do {
            if ((*q)[0xd3] == 4) n++;
            q++;
            c--;
        } while (c != 0);
    }
    return n;
}
