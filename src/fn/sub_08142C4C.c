#define Player_SetActionOrigin sub_08142C4C
#include "global.h"
#include "player.h"
void sub_08142C4C(struct Player *p)
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
