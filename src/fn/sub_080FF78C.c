#include "global.h"

void sub_080FF78C(void *p, u16 flags)
{
    *(u16 *)((u8 *)p + 0xC8) |= flags;
}
