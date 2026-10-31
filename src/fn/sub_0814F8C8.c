#include "player_cue.h"

void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_UpdateHidden(struct Player *p)
{
    Player_AddControlFlags(p, 5);
}
