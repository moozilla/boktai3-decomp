#include "global.h"

s32 Player_SetAnimation(u8 *, s32, s32, s32);
void Player_UpdateFacingSide(u8 *);
void PlayerCue_EnterStandUpdate(u8 *);
void PlayerCue_CompleteAction(u8 *);

void PlayerCue_UpdateStandDiagonal(u8 *p)
{
    if (Player_SetAnimation(p, 0x161, 0x40, 1) != 0) {
        Player_UpdateFacingSide(p);
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
    }
}
