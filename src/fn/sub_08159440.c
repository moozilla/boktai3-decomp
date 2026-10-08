#include "global.h"
void sub_08159440(u8 *p, u32 v)
{
    *(u32 *)(p + 0x47c) = v;
    p[0x478] = 1;
}
