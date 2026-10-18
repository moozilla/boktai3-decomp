#include "global.h"
/* Spider_TickActionTimer: Clears the queued flag and unconditionally increments the action timer */
#define Spider_TickActionTimer sub_080A3804

void sub_080A3804(u8 *p)
{
    if (p[0x2b1])
        p[0x2b1] = 0;
    (*(u32 *)(p + 0x2ac))++;
}
