// COMPILER: old_agbcc
#include "global.h"
/* Anim_ResetDurationScale: Restores the base frame duration and resets its scale to 64 */

void Anim_ResetDurationScale(u8 *p)
{
    p[7] = p[6];
    *(u16 *)(p + 0xc) = 64;
}
