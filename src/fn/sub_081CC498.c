#include "global.h"
void sub_0802DCD8(u8 *, u32, u32, u32, u32);
void sub_081CC498(u8 *a, u8 *b)
{
    if (b[9]) {
        u32 z = 0;
        b[9] = z;
        b[7] = z;
        sub_0802DCD8(b, 3, 1, 0, 4);
        b[0x18] = z;
        *(u32 *)(b + 0x10) = z;
        b[7] = 1;
    }
}
