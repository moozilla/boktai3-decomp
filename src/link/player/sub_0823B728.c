#include "global.h"
#include "player/player.h"

s32 Player_CanUseAction(struct Player *, s32);
void Player_InitFacingDriftOffset(struct Player *);
void ActorMotion_ApplyHeadingDrift(struct Player *);
void Player_ClearMovementLock(struct Player *);

void sub_0823B728(struct Player *p, s32 action)
{
    if ((p->inputKeys[0] & 0x100) && Player_CanUseAction(p, action)) {
        if ((p->actionMode <= 1 || p->actionMode == 6) && p->tetherMode == 4)
            Player_InitFacingDriftOffset(p);
        else
            ActorMotion_ApplyHeadingDrift(p);
    } else if (p->cameraOffsetActive != 0) {
        Player_ClearMovementLock(p);
    }
    p->cameraInputProcessed = 1;
}
