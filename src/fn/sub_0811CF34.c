#include "global.h"
u8 sub_08112BA8(u32);
void sub_0811CF34(u8 *p, u32 x)
{
    *(u8 *)(p + 0x250) = sub_08112BA8(x);
}
