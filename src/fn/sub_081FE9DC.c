#include "global.h"
#include "ram.h"
struct G {
    u8 p[0xeec];
    s32 w;
};
void SunOrb_GetOrCreateManager(void);
s32 SunOrb_GetManagerStatus(void)
{
    struct G *g = gSlotController;
    s32 r;
    if (g != 0)
        r = g->w;
    else {
        SunOrb_GetOrCreateManager();
        r = -1;
    }
    return r;
}
