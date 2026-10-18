// COMPILER: old_agbcc
#include "global.h"
/* Actor_SetRequestedAction: Stores the requested action identifier at offset 0x12E; interpretation tentative */
#define Actor_SetRequestedAction sub_0802D014

void sub_0802D014(u8 *p, u16 v)
{
    *(u16 *)(p + 0x12e) = v;
}
