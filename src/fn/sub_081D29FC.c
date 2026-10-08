#include "global.h"

void sub_081D2610(u8 *);
void sub_0802D9F8(u32, u8 *);

s32 sub_081D29FC(u32 a, u8 *b)
{
    s32 z;
    sub_081D2610(b);
    sub_0802D9F8(a, b);
    z = 0;
    b[5] = z;
    return z;
}
