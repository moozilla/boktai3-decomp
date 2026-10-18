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
void Player_UpdateFacingSide(void *);
void Player_UpdateThreePhaseBaseAnimationAction(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, 0, 0x40, 1))
                ActorSequence_Begin(p, 0x11, 1);
            break;
        case 1:
            Player_SetAnimation(p, 0, 0x40, 1);
            if (!p->tetherFrame) {
                PlayerCue_CompleteAction(p);
                p->tetherFrame++;
            }
            break;
        case 2:
            if (Player_SetAnimation(p, 0, 0x40, 1)) {
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
