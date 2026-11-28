#include "global.h"
#include "player/player.h"

void Player_OnFadeMotionExit(struct Player *p)
{
    struct AnimSprite *animation = &p->animation;

    animation->flags &= ~2;
}
