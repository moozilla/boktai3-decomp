#include "global.h"
#include "ram.h"
struct G {
    u8 f[0x58];
    u8 n;
};
struct P {
    u32 fl;
    u8 f[0x50 - 4];
    u16 h;
};
void Hresvelgr_ReleaseEffect(struct P *p)
{
    struct G *g = gMotionEntryManager;
    if (p != 0) {
        g->n--;
        p->h = 0;
        p->fl |= 1;
    }
}
