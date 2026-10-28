#include "global.h"
#include "player.h"

void Player_NotifyActionInterruption(u8 *);
void ActorSequence_Begin(u8 *, s32, s32);

s32 Player_EnterStateTen(void)
{
    u8 *g = (*(u8 **)gPlayers);

    if (g != 0) {
        Player_NotifyActionInterruption(g);
        ActorSequence_Begin(g, 0xA, 0);
    }
    return 0;
}
