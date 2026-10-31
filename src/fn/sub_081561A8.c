#include "player_cue.h"

s32 Player_SetAnimation(void *, u32, u32, u32);
void Player_UpdateFacingSide(void *);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);

void PlayerCue_UpdateStandDiagonalB(struct Player *p)
{
    if (Player_SetAnimation(p, 0x1BB, 0x40, 1) != 0) {
        p->tetherPhase++;
        if (p->tetherPhase > 1) {
            Player_UpdateFacingSide(p);
            PlayerCue_EnterStandUpdate(p);
            PlayerCue_CompleteAction(p);
        }
    }
}
