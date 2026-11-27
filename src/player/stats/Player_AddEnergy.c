#include "player/player.h"

void Player_AddEnergy(struct Player *p, u32 d)
{
    if (p->kind == 1) {
        p->currentStatB += d;
        if (p->currentStatB >= p->maximumStatB)
            p->currentStatB = p->maximumStatB;
    }
}
