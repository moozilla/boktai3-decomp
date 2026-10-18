#include "global.h"

extern u8 *gUnk_02000580;
void Player_NotifyActionInterruption(u8 *);
void ActorSequence_Begin(u8 *, s32, s32);

s32 Player_EnterStateTen(void)
{
    u8 *g = gUnk_02000580;

    if (g != 0) {
        Player_NotifyActionInterruption(g);
        ActorSequence_Begin(g, 0xA, 0);
    }
    return 0;
}
