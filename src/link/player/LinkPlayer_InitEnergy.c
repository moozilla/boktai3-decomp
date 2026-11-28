#include "global.h"
#include "player/player.h"

void LinkPlayer_InitEnergy(struct Player *p)
{
    p->currentStatB = p->maximumStatB = p->derivedStats[1] * 5 + 100;
}
