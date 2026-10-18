#include "global.h"
/* Vampire_GetBarrierCount: Getter for the same offset-0x384 counter that Vampire_DecrementBarrierCount decrements */
#define Vampire_GetBarrierCount sub_08028DFC

u8 *SceneActors_GetBySlot(u32);

u8 sub_08028DFC(void)
{
    u8 *p = SceneActors_GetBySlot(0);
    u8 r;
    if (p)
        r = p[0x384];
    else
        r = 0;
    return r;
}
