#include "global.h"
#include "player.h"
void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void sub_08153E1C(struct Player *);
void sub_0815093C(struct Player *);
void PlayerCue_BeginMode27(struct Player *p, s32 facing, s32 variant, u32 parameter)
{
    if (facing >= 0)
        p->facing = facing;
    p->facing |= 1;
    p->cueParameter = parameter;
    p->animationBranchRequested = variant;
    p->cueVariant = variant;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0x1B, 0);
    if (p->actionMode == 4) {
        p->triggerAnimationCallback = sub_08153E1C;
    } else {
        if (p->actionMode == 5) {
            p->actionLatch464 = 1;
            p->actionLatch465 = 1;
        }
        p->triggerAnimationCallback = sub_0815093C;
    }
}
