#define Player_BeginForcedDirection sub_08158DF0
#include "global.h"
#include "player.h"
void sub_0813AFEC(struct Player *, s32, s32);
void sub_08158DF0(struct Player *p, s32 direction)
{
    sub_0813AFEC(p, 4, 1);
    p->coordinateAxis = direction;
    direction += 4;
    {
        s16 mask = 7;
        direction &= mask;
        p->facing = direction;
    }
    if (p->coordinateAxis > 4) {
        p->heading = (8 - p->coordinateAxis) >> 1;
        p->side = 1;
    } else {
        p->heading = p->coordinateAxis >> 1;
        p->side = 0;
    }
    p->directionFrame = 0;
}
