#include "player.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_TurnTowardTargetFacing(struct Player *);

void PlayerCue_Face(struct Player *p, u8 b, u32 c)
{
    p->targetFacing = b;
    p->cueParameter = c;
    p->triggerAnimationCallback = Player_TurnTowardTargetFacing;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 0);
}
