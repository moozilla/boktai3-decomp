#include "global.h"
/* Count_GetBarrierCount: Getter for the same offset-0x384 counter that Count_DecrementBarrierCount decrements */

u8 *SceneActors_GetBySlot(u32);

u8 Count_GetBarrierCount(void)
{
    u8 *p = SceneActors_GetBySlot(0);
    u8 r;
    if (p)
        r = p[0x384];
    else
        r = 0;
    return r;
}
