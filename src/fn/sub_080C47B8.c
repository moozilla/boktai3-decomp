#include "global.h"

#ifdef NONMATCHING
// fuzzy: 94.4%
// foundry draft (worker best 94.4%); what differs is mostly register allocation and instruction order
void sub_080C47B8(void *ptr)
{
    u16 *p = ptr;
    u32 mask = 0xFFFFFF00;

    *p = (*p & mask) + 0x80;
    p[1] = (mask & p[1]) + 0x80;
    p[2] = (mask & p[2]) + 0x80;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C47B8);
#endif
