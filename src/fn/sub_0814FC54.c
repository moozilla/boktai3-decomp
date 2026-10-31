#include "player_cue.h"

s32 ActorVariant_GetTableValue(s32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_AddControlFlags(struct Player *, s32);

void PlayerCue_UpdateFaceSide(struct Player *p)
{
    s32 r;

    r = ActorVariant_GetTableValue(p->actionMode, 0x34);
    Player_SetAnimation(p, r, 0x40, 1);
    Player_AddControlFlags(p, 4);
}
