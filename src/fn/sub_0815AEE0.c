#include "player.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdateHidden(struct Player *);

void PlayerCue_Hide(struct Player *p)
{
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 0);
    p->animation.flags |= 1;
    p->triggerAnimationCallback = PlayerCue_UpdateHidden;
}
