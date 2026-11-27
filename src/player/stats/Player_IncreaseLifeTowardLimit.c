#include "player/player.h"

void Player_IncreaseLifeTowardLimit(struct Player *p, u32 d)
{
    if (p->kind == 1) {
        p->currentStatA += d;
        if (p->currentStatA >= p->maximumStatA)
            p->currentStatA = p->maximumStatA;
    }
}
