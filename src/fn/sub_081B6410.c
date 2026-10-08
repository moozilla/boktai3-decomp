#include "global.h"
void sub_081B6410(u8 *p, u32 v)
{
    *(u16 *)(p + 0x188) = v;
    *(u16 *)(p + 0x3A4) = v;
}
