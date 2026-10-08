#include "global.h"
u32 sub_08013930(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void sub_08013B98(u32, u32, u32);
u32 sub_08013A24(u32 a, u32 b)
{
    u32 r = sub_08013930(a, b, 0x40, 0x10, 0x40, 0, 0xa00, 1, 5, 1, 0);
    if (r == 0)
        sub_08013B98(a, 0x1c1b, 6);
    return r;
}
