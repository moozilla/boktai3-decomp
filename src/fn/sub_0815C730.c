#include "player.h"

void Player_EmitQueuedEffects(struct Player *);

void PlayerCue_Refresh(struct Player *p)
{
    Player_EmitQueuedEffects(p);
}
