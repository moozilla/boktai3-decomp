#include "global.h"
void sub_080B12AC(u8 *p)
{
    u16 *q = (u16 *)(p + 0x6d4);
    u32 m = ~8;
    *q = m & *q;
}
