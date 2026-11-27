#include "player/player.h"

void Player_SubtractLifeNonlethal(struct Player *p, s32 d)
{
    if (p->kind == 1) {
        if (p->currentStatA < d + 1)
            p->currentStatA = p->kind;
        else
            p->currentStatA -= d;
    }
}
