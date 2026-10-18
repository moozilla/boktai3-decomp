#include "global.h"

s32 sub_081419D4(s32, s32);
s32 sub_0813AE18(u8 *, s32, s32, s32);
void Player_AddControlFlags(u8 *, s32);

void PlayerCue_UpdateFaceSide(u8 *p)
{
    s32 r;

    r = sub_081419D4(p[0x418], 0x34);
    sub_0813AE18(p, r, 0x40, 1);
    Player_AddControlFlags(p, 4);
}
