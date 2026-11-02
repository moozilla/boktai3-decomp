#include "global.h"
#include "player.h"
#include "sun_orb_effect.h"

s32 OrbitEffect_UpdateContraction(struct OrbitEffect *p, s32 slot)
{
    u32 r;
    if ((*(u8 **)gPlayers)[0x457] != 0x1a)
        p->scale -= 0x4000;
    if (p->scale > 0)
        r = 0;
    else {
        p->scale = 0;
        r = 1;
    }
    return r;
}
