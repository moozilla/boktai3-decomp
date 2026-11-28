#include "global.h"
#include "player/player.h"

void PlayerAction_RefreshModifiers(void *);

void Player_OnMotion11Exit(struct Player *p)
{
    p->auxiliarySprite[0].sprite.flags |= 1;
    p->formCueParameterA = 0;
    p->terrainRecoveryCounter = 0;
    p->formCueParameterB = 0;
    p->unk432[1] = 0;
    PlayerAction_RefreshModifiers(p);
    p->baseActionVariant = 0xFF;
}
