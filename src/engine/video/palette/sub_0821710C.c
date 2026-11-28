#include "global.h"
void sub_0821710C(s32 pal, s32 idx, u16 color)
{
    ((u16 *)OBJ_PLTT)[pal * 16 + idx] = color;
}
