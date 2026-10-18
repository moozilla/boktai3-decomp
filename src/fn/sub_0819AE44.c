#include "global.h"

void ActionState_Reset(u8 *p)
{
    u32 z = 0;
    *(p + 2) = z;
    *(p + 1) = z;
    *p = z;
    *(u32 *)(p + 0xC) = z;
    *(u32 *)(p + 8) = z;
    *(u32 *)(p + 4) = z;
}
