#include "global.h"
#include "player.h"
#define Player_ResetTriggerAnimation  PlayerCue_EnterStandUpdate
#define Player_DispatchPendingTrigger PlayerCue_CompleteAction
void Sound_PlaySong(u32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
s32 ActorVariant_GetTableValue(u32, s32);
void PlayerCue_EnterStandUpdate(struct Player *);
void PlayerCue_CompleteAction(struct Player *);
void Player_UpdateSongCueAnimation(struct Player *p)
{
    switch ((s32)p->tetherPhase) {
        case 0:
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 0x40, 1)) {
                Sound_PlaySong(0xCB);
                if (p->animationBranchRequested)
                    p->tetherPhase = 1;
                else
                    p->tetherPhase = 2;
                p->animationBranchRequested = 0;
                p->tetherFrame = 0;
            }
            break;
        case 1:
            p->resetValue6FC = 30;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 9), 0x40, 1);
            PlayerCue_CompleteAction(p);
            break;
        case 2:
            p->resetValue6FC = 30;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 0x40, 1);
            PlayerCue_CompleteAction(p);
            break;
        case 3:
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 0x40, 1)) {
                u32 facing = p->facing;
                if (facing > 4) {
                    p->heading = 8 - facing;
                    p->side = 1;
                } else {
                    p->heading = facing;
                    p->side = 0;
                }
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
