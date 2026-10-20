#include "global.h"
#include "player.h"
void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdateSideArrival(struct Player *);
void Player_UpdateTimedDirectionAction(struct Player *);
void PlayerCue_BeginMode28(struct Player *p, s32 facing, s32 variant, u32 parameter)
{
    if (facing >= 0)
        p->facing = facing;
    p->facing |= 1;
    p->cueParameter = parameter;
    Player_EnterKindTwo(p);
    p->terrainRecoveryCounter = variant;
    ActorSequence_Begin(p, 0x1C, 0);
    if (p->actionMode == 4) {
        p->triggerAnimationCallback = PlayerCue_UpdateSideArrival;
    } else {
        if (p->actionMode == 5) {
            p->actionLatch464 = 1;
            p->actionLatch465 = 1;
        }
        p->triggerAnimationCallback = Player_UpdateTimedDirectionAction;
    }
}
