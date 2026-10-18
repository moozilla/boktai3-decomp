#include "global.h"

void Player_StopActionSounds(u8 *);
void Player_RestoreCameraFollow(u8 *);

void Player_StopMovementEffects(u8 *p)
{
    Player_StopActionSounds(p);
    if (*(s16 *)(p + 0x4B6) != 0) {
        Player_RestoreCameraFollow(p);
    }
}
