#include "global.h"
u16 *sub_0805DD18(u32, u32, u32);
void sub_0805DD38(u32 a, u32 b, s32 n)
{
    u16 *q = sub_0805DD18(0, a, b);
    if (n > 0) {
        do {
            *q = 0xF001;
            q++;
            n--;
        } while (n > 0);
    }
}
