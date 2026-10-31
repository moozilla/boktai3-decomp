#include "player_cue.h"

void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateTriggerAnimation(struct Player *);
void PlayerCue_EnterStandUpdate(struct Player *p)
{
    ActorSequence_Begin(p, 0, 0);
    p->triggerAnimationCallback = Player_UpdateTriggerAnimation;
}
