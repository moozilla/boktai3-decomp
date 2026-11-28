#include "global.h"
#include "player/player.h"

void LinkPlayer_InitHealth(struct Player *p)
{
    p->currentStatA = p->maximumStatA = p->derivedStats[0] * 10;
}
