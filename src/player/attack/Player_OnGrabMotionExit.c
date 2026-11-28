#include "global.h"
#include "player/player.h"

void Player_OnGrabMotionExit(struct Player *p)
{
    AnimSprite_Hide(&p->secondaryAnimation);
}
