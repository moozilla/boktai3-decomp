#include "global.h"

void ActorSequence_Begin(u8 *, s32, s32);
void Player_UpdateTriggerAnimation(void);

void PlayerCue_EnterStandUpdate(u8 *p)
{
    ActorSequence_Begin(p, 0, 0);
    *(void (**)(void))(p + 0x58C) = Player_UpdateTriggerAnimation;
}
