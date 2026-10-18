// COMPILER: old_agbcc
#include "global.h"
/* Anim_ResetDurationScale: Restores the base frame duration and resets its scale to 64 */
#define Anim_ResetDurationScale sub_08220D6C

void sub_08220D6C(u8 *p)
{
    p[7] = p[6];
    *(u16 *)(p + 0xc) = 64;
}
