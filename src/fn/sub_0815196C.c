#include "global.h"

void sub_0815196C(u8 *p)
{
    u16 *q = (u16 *)(p + 0xAD6);
    u16 v = 0xBA;

    v <<= 1;
    *q = v;
    *(u16 *)(p + 0xAD8) = 0x18;
}
