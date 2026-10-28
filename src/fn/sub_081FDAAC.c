#include "global.h"
#include "player.h"
struct S {
    u8 pad[0x3a4];
    s32 w3a4;
};
u32 OrbitEffect_UpdateContraction(struct S *p)
{
    u32 r;
    if ((*(u8 **)gPlayers)[0x457] != 0x1a)
        p->w3a4 -= 0x4000;
    if (p->w3a4 > 0)
        r = 0;
    else {
        p->w3a4 = 0;
        r = 1;
    }
    return r;
}
