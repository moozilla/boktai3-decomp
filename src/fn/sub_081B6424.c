#include "global.h"
void sub_081B6424(u8 *p, u32 v)
{
    *(u16 *)(p + 0x43E) = v;
    *(u16 *)(p + 0x222) = v;
}
