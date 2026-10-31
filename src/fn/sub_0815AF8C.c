#include "player.h"

void Player_UpdateFacingSide(struct Player *);
void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateTriggerAnimation(struct Player *);

void PlayerCue_Idle(struct Player *p, s32 v)
{
    if (v >= 0)
        p->facing = v;
    Player_UpdateFacingSide(p);
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 1);
    p->triggerAnimationCallback = Player_UpdateTriggerAnimation;
}
