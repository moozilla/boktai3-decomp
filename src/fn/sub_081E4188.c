#include "global.h"
void sub_081E4188(u8 *p)
{
    u32 a = *(u8 *)(p + 0xF86);
    u32 b;
    *(u16 *)(p + 0xE2C) = a;
    b = *(u8 *)(p + 0xF87);
    *(u16 *)(p + 0xE32) = b;
}
