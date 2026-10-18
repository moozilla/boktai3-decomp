#include "global.h"

void WoodenCrate_SetFlags(void *p, u16 flags)
{
    *(u16 *)((u8 *)p + 0xC8) |= flags;
}
