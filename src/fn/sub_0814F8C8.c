#include "global.h"

void Player_AddControlFlags(u8 *, s32);

void PlayerCue_UpdateHidden(u8 *p)
{
    Player_AddControlFlags(p, 5);
}
