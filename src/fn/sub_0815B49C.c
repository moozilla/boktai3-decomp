#define PlayerCue_BeginMode28 sub_0815B49C
#define Player_EnterKindTwo   sub_0815AD78
#include "global.h"
#include "player.h"
void sub_0815AD78(struct Player *);
void sub_0813AFEC(struct Player *, s32, s32);
void sub_081540D4(struct Player *);
void sub_08150B34(struct Player *);
void sub_0815B49C(struct Player *p, s32 facing, s32 variant, u32 parameter)
{
    if (facing >= 0)
        p->facing = facing;
    p->facing |= 1;
    p->cueParameter = parameter;
    sub_0815AD78(p);
    p->terrainRecoveryCounter = variant;
    sub_0813AFEC(p, 0x1C, 0);
    if (p->actionMode == 4) {
        p->triggerAnimationCallback = sub_081540D4;
    } else {
        if (p->actionMode == 5) {
            p->actionLatch464 = 1;
            p->actionLatch465 = 1;
        }
        p->triggerAnimationCallback = sub_08150B34;
    }
}
