#include "global.h"

void ActionState_AdvanceState(u8 *p)
{
    u32 z;
    u8 v = *p + 1;
    z = 0;
    *p = v;
    *(p + 2) = z;
    *(p + 1) = z;
    *(u32 *)(p + 0xC) = z;
    *(u32 *)(p + 8) = z;
    *(u32 *)(p + 4) = z;
}
