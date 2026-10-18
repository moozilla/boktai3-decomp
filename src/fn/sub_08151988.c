#define Player_ResetDefaultAction PlayerCue_EnterStandUpdate
#define Player_ConsumePendingActionTrigger PlayerCue_CompleteAction
#define Player_UpdateThreePhaseBaseAnimationAction sub_08151988
#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(void *, s32, s32, s32);
void sub_0813AFEC(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void sub_0813ADD0(void *);
void sub_08151988(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            if (sub_0813AE18(p, 0, 0x40, 1))
                sub_0813AFEC(p, 0x11, 1);
            break;
        case 1:
            sub_0813AE18(p, 0, 0x40, 1);
            if (!p->tetherFrame) {
                PlayerCue_CompleteAction(p);
                p->tetherFrame++;
            }
            break;
        case 2:
            if (sub_0813AE18(p, 0, 0x40, 1)) {
                sub_0813ADD0(p);
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
