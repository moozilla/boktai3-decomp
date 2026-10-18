#include "global.h"
#include "player.h"
#define Player_ResetTriggerAnimation  PlayerCue_EnterStandUpdate
#define Player_DispatchPendingTrigger PlayerCue_CompleteAction
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Player_UpdateTriggerAnimation(struct Player *p)
{
    switch ((s32)p->tetherPhase) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40, 1);
            if (p->pendingAnimationTrigger) {
                p->pendingAnimationTrigger = 0;
                goto dispatch;
            }
            break;
        case 2:
            if (++p->tetherFrame > 19)
                goto reset;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2F), 0x40, 1);
            break;
        reset:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40, 1);
            PlayerCue_EnterStandUpdate(p);
        dispatch:
            PlayerCue_CompleteAction(p);
            break;
        case 3:
            if (p->tetherFrame <= 19) {
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x30), 0x40, 2);
                p->tetherFrame++;
            } else {
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x30), 0x40, 2);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
