#include "global.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);
s32 SunOrb_GetSlotState(s32 i)
{
    u8 *g = gSlotController;
    s32 r;
    s32 off;
    u8 *q;
    if (g != 0) {
        off = i * 0x3ac;
        q = g + 0x38;
        r = *(s32 *)(q + off);
    } else {
        SunOrb_GetOrCreateManager();
        r = -1;
    }
    return r;
}
