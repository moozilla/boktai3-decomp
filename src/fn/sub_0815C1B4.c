#include "player.h"
void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, u32, u32);
void Player_UpdateTransitionAnimation(struct Player *);
void PlayerCue_WindblownVariant2(struct Player *p, s32 a, u32 b)
{
    if (a != 0) {
        u8 *q = &p->facing;
        u8 z;
        z = 0;
        *q = 5;
        p->heading = z;
        p->side = 1;
    } else {
        p->facing = 3;
        p->heading = a;
        p->side = a;
    }
    p->cueParameter = b;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 2);
    p->triggerAnimationCallback = Player_UpdateTransitionAnimation;
}
