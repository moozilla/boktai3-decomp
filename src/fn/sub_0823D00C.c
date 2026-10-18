#include "global.h"
#include "player.h"

/* Tentative: position offset used as the player's action origin. */
void Player_SetActionOrigin_0823D00C(struct Player *p)
{
    p->actionOrigin = p->position;
    if (p->facing == 4) {
        p->actionOrigin.x -= 20;
        p->actionOrigin.y += 330;
        p->actionOrigin.z += 60;
    } else {
        p->actionOrigin.x += 20;
        p->actionOrigin.y += 330;
        p->actionOrigin.z -= 30;
    }
}
