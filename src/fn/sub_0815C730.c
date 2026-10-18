#include "global.h"

void Player_EmitQueuedEffects(u8 *);

void PlayerCue_Refresh(u8 *p)
{
    Player_EmitQueuedEffects(p);
}
