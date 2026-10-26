#include "global.h"
#include "ram.h"
struct G {
    u8 f[0x3c];
    s32 n;
};
struct P {
    u32 fl;
    u8 f[0x8a - 4];
    u16 h;
};
void Hresvelgr_ReleaseShot(struct P *p)
{
    struct G *g = gDirectionBurstEmitter;
    if (p != 0) {
        g->n--;
        p->fl |= 1;
        p->h = 0;
        p->fl &= 0xFFFFF9FF;
    }
}
