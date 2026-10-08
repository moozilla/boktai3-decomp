#include "global.h"

void sub_081C6780(u8 *);
void sub_0802D9F8(u32, u8 *);

s32 sub_081C6918(u32 a, u8 *b)
{
    s32 z;
    sub_081C6780(b);
    sub_0802D9F8(a, b);
    z = 0;
    b[5] = z;
    return z;
}
