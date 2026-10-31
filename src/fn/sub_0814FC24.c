#include "player_cue.h"

s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_UpdateFacingSide(struct Player *);
void PlayerCue_EnterStandUpdate(struct Player *);
void PlayerCue_CompleteAction(struct Player *);

void PlayerCue_UpdateStandDiagonal(struct Player *p)
{
    if (Player_SetAnimation(p, 0x161, 0x40, 1) != 0) {
        Player_UpdateFacingSide(p);
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
    }
}
