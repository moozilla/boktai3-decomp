#include "global.h"
#include "player/player.h"

void Player_OnTargetMotionExit(struct Player *p)
{
    p->animation.position = *(struct ContactPosition *)&p->position;
    AnimSprite_Hide(&p->secondaryAnimation);
}
