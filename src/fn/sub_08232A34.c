#include "global.h"

void Effect_Remove(u32 p);

void sub_08232A34(u32 unused, u8 *p)
{
    Effect_Remove((u32)(p + 0x8));
}
