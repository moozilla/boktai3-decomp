// COMPILER: old_agbcc
#include "global.h"
/* Pool184_RemoveStatusFlags: Removes bits from the status flags of a 0x184-byte pooled object */
#define Pool184_RemoveStatusFlags sub_081041F0

void sub_081041F0(u8 *p, u16 mask)
{
    *(u16 *)(p + 0x156) &= ~mask;
}
