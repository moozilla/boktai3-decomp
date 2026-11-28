#include "global.h"
#include "player/player.h"

void Player_StopActionSounds(struct Player *);
void Player_RestoreCameraFollow(struct Player *);

void Player_StopMovementEffects(struct Player *p)
{
    Player_StopActionSounds(p);
    if (p->cameraOffsetActive != 0)
        Player_RestoreCameraFollow(p);
}
