#include "global.h"

s32 ActorVariant_GetTableValue(s32, s32);
s32 Player_SetAnimation(u8 *, s32, s32, s32);
void Player_AddControlFlags(u8 *, s32);

void PlayerCue_UpdateFaceSide(u8 *p)
{
    s32 r;

    r = ActorVariant_GetTableValue(p[0x418], 0x34);
    Player_SetAnimation(p, r, 0x40, 1);
    Player_AddControlFlags(p, 4);
}
