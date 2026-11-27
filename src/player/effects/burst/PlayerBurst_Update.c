#include "global.h"

struct P {
    u8 f00[0x4C4];
    u32 w;
};

void PlayerOrbitEffect_UpdateParticles(struct P *);
void Callback_InvokeActor(struct P *, u32);

s32 PlayerBurst_Update(struct P *p)
{
    PlayerOrbitEffect_UpdateParticles(p);
    Callback_InvokeActor(p, p->w);
    return 0;
}
