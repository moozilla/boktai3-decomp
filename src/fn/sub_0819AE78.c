// COMPILER: old_agbcc
#include "global.h"
/* ActionState_ResetSubstate: Clears substate bytes and two scratch words while preserving the main state and first counter */
#define ActionState_ResetSubstate sub_0819AE78

void sub_0819AE78(u8 *p)
{
    u32 z = 0;
    p[2] = z;
    p[1] = z;
    *(u32 *)(p + 0xc) = z;
    *(u32 *)(p + 8) = z;
}
