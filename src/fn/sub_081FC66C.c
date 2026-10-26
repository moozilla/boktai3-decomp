#include "global.h"
#include "ram.h"
struct G {
    u8 p[0x1c];
    u32 w1c;
};
u32 SceneProjectile_ConsumeHitEvent(void)
{
    struct G *g = gSceneProjectilePool;
    u32 r;
    if (g->w1c == 0)
        r = 0;
    else {
        g->w1c = 0;
        r = 1;
    }
    return r;
}
