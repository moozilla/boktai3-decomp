#include "global.h"
#include "player/player.h"

void ActorSequence_Begin(struct Player *, u32, u32);

void Player_ResetIdleMotion(struct Player *p)
{
    p->statusFlags &= ~1;
    if (!p->movementModifierActive)
        ActorSequence_Begin(p, 0, 0);
}
