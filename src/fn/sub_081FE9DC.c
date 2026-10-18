#include "global.h"
struct G {
    u8 p[0xeec];
    s32 w;
};
extern struct G *gUnk_020005EC;
void SunOrb_GetOrCreateManager(void);
s32 SunOrb_GetManagerStatus(void)
{
    struct G *g = gUnk_020005EC;
    s32 r;
    if (g != 0)
        r = g->w;
    else {
        SunOrb_GetOrCreateManager();
        r = -1;
    }
    return r;
}
