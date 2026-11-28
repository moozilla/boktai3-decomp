#include "global.h"
#include "player/player.h"

void Player_RestoreCameraFollow(struct Player *);

void sub_08156C8C(struct Player *p)
{
    if (p->cameraOffsetActive != 0)
        Player_RestoreCameraFollow(p);
}
