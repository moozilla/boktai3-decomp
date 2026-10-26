#include "global.h"
#include "ram.h"

struct S {
    u8 pad[0x1f];
    u8 b1f;
};

s32 PlayerParticleCloud_RequestStop(void)
{
    struct S *p = gCloudTask;
    s32 r;
    if (p) {
        p->b1f = 1;
        r = 0;
    } else {
        r = -1;
    }
    return r;
}
