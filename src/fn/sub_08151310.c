#define Player_ResetDefaultAction PlayerCue_EnterStandUpdate
#define Player_ConsumePendingActionTrigger PlayerCue_CompleteAction
#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Player_UpdateSixPhaseAction(struct Player *p)
{
    if (p->actionMode == 6)
        p->actionVariant = 9;
    else
        p->actionVariant = 8;
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 0x40, 1)) {
                Sound_PlaySong(0x1F7);
                ActorSequence_Begin(p, 3, 1);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40, 1);
            break;
        case 2:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40, 1))
                ActorSequence_Begin(p, 3, 3);
            break;
        case 3:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 0x40, 1))
                ActorSequence_Begin(p, 3, 4);
            break;
        case 4:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40, 1);
            if (++p->tetherFrame > 14)
                ActorSequence_Begin(p, 3, 5);
            break;
        case 5:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 0x40, 1)) {
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
