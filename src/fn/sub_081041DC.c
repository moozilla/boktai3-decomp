#include "global.h"

void sub_081041DC(u8 *obj, u16 flags)
{
    *(u16 *)(obj + 0x156) |= flags;
}
