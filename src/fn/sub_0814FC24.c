#include "global.h"

s32 sub_0813AE18(u8 *, s32, s32, s32);
void sub_0813ADD0(u8 *);
void PlayerCue_EnterStandUpdate(u8 *);
void PlayerCue_CompleteAction(u8 *);

void PlayerCue_UpdateStandDiagonal(u8 *p)
{
    if (sub_0813AE18(p, 0x161, 0x40, 1) != 0) {
        sub_0813ADD0(p);
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
    }
}
