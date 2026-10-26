#include "global.h"

struct P {
    u8 f00[0x418];
    u8 d18;
};
struct G {
    u8 f00[0x600];
    u32 w600;
};
extern struct G *gGameState;

void Player_OnStateOneEvent(u32 a, u32 b, struct P *p)
{
    if (p->d18 == 1)
        gGameState->w600 += 4;
}
