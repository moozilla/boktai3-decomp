#include "global.h"

void Pool184_AddStatusFlags(u8 *obj, u16 flags)
{
    *(u16 *)(obj + 0x156) |= flags;
}
