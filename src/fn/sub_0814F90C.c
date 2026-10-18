#include "global.h"
#include "player.h"
#define Player_UpdateTriggerAnimation sub_0814F90C
#define Player_ResetTriggerAnimation PlayerCue_EnterStandUpdate
#define Player_DispatchPendingTrigger PlayerCue_CompleteAction
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void sub_0814F90C(struct Player *p)
{
    switch ((s32)p->tetherPhase) {
        case 0:
            sub_0813AE18(p, sub_081419D4(p->actionMode, 0), 0x40, 1);
            if (p->pendingAnimationTrigger) {
                p->pendingAnimationTrigger = 0;
                goto dispatch;
            }
            break;
        case 2:
            if (++p->tetherFrame > 19)
                goto reset;
        case 1:
            sub_0813AE18(p, sub_081419D4(p->actionMode, 0x2F), 0x40, 1);
            break;
        reset:
            sub_0813AE18(p, sub_081419D4(p->actionMode, 0), 0x40, 1);
            PlayerCue_EnterStandUpdate(p);
        dispatch:
            PlayerCue_CompleteAction(p);
            break;
        case 3:
            if (p->tetherFrame <= 19) {
                sub_0813AE18(p, sub_081419D4(p->actionMode, 0x30), 0x40, 2);
                p->tetherFrame++;
            } else {
                sub_0813AE18(p, sub_081419D4(p->actionMode, 0x30), 0x40, 2);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
