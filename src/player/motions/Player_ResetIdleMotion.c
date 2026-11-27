#include "global.h"
void ActorSequence_Begin(u8 *, u32, u32);
void Player_ResetIdleMotion(u8 *p)
{
    *(u32 *)(p + 0x20) &= ~1;
    if (p[0x5bb] == 0)
        ActorSequence_Begin(p, 0, 0);
}
