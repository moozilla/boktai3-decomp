#define Player_ResetDefaultAction PlayerCue_EnterStandUpdate
#define Player_ConsumePendingActionTrigger PlayerCue_CompleteAction
#define Player_UpdateSixPhaseAction        sub_08151310
#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(void *, s32, s32, s32);
void sub_0813AFEC(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void sub_08151310(struct Player *p)
{
    if (p->actionMode == 6)
        p->actionVariant = 9;
    else
        p->actionVariant = 8;
    switch (p->tetherPhase) {
        case 0:
            if (sub_0813AE18(p, sub_081419D4(p->actionMode, 0x23), 0x40, 1)) {
                Sound_PlaySong(0x1F7);
                sub_0813AFEC(p, 3, 1);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 1:
            sub_0813AE18(p, sub_081419D4(p->actionMode, 0x24), 0x40, 1);
            break;
        case 2:
            if (sub_0813AE18(p, sub_081419D4(p->actionMode, 0x24), 0x40, 1))
                sub_0813AFEC(p, 3, 3);
            break;
        case 3:
            if (sub_0813AE18(p, sub_081419D4(p->actionMode, 0x25), 0x40, 1))
                sub_0813AFEC(p, 3, 4);
            break;
        case 4:
            sub_0813AE18(p, sub_081419D4(p->actionMode, 0x24), 0x40, 1);
            if (++p->tetherFrame > 14)
                sub_0813AFEC(p, 3, 5);
            break;
        case 5:
            if (sub_0813AE18(p, sub_081419D4(p->actionMode, 0x23), 0x40, 1)) {
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
