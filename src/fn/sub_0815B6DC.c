#include "player.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdateRecoilRecovery(struct Player *);

void PlayerCue_Motion29B(struct Player *p, s32 dir, u32 h, u32 c)
{
    if (dir >= 0)
        p->facing = dir;
    p->facing |= 1;
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = p->facing >> 1;
        p->side = 0;
    }
    p->recoilSpeed = h;
    p->cueParameter = c;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0x1D, 0);
    p->triggerAnimationCallback = PlayerCue_UpdateRecoilRecovery;
}
