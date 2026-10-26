#include "global.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);
void OrbitEffect_UpdateContraction(void);
s32 SunOrb_IsFadingOut(s32 i)
{
    u8 *g = gSlotController;
    s32 r;
    s32 off;
    u8 *q;
    if (g == 0) {
        SunOrb_GetOrCreateManager();
        return -1;
    }
    r = 0;
    off = i * 0x3ac;
    q = g + 0x68;
    if (*(void **)(q + off) == (void *)OrbitEffect_UpdateContraction)
        r = 1;
    return r;
}
