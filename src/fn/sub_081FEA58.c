#include "global.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);
u8 *SunOrb_GetPosition(s32 i)
{
    u8 *g = gSlotController;
    u8 *r;
    if (g != 0)
        r = (u8 *)(i * 0x3ac + (u32)g) + 0x88;
    else {
        SunOrb_GetOrCreateManager();
        r = 0;
    }
    return r;
}
