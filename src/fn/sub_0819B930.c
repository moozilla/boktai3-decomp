#include "global.h"
#include "ram.h"
extern u8 *gGameState;
s32 AreaEffect_IsPositionCovered(void *);
s32 Hresvelgr_GetEnvironmentalLevel(u8 *p)
{
    s32 r;
    if (gPlayerEffectPendingCount != 0) {
        r = 4;
    } else if (AreaEffect_IsPositionCovered(p + 0x54) == 0) {
        r = 0;
    } else {
        r = *(s16 *)(gGameState + 0x876);
    }
    return r;
}
