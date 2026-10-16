#include "global.h"

void Route_Start(u32, u8, u8, u8);

u32 sub_08225B4C(u8 *p, u32 a, u8 b, u8 c, u8 d)
{
    *(u32 *)(p + 0x28) = a;
    Route_Start(a, b, c, d);
    return 1;
}
