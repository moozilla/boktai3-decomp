#include "global.h"
#include "engine/engine.h"
#include "player/player.h"

void PlayerAction_RefreshModifiers(void *);

void Player_OnMotion10Exit(struct Player *p)
{
    if (!(gEngineFlags & 0x1000))
        PlayerAction_RefreshModifiers(p);
    p->baseActionVariant = 0xFF;
}
