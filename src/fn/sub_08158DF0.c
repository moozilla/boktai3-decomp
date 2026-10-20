#include "global.h"
#include "player.h"

void ActorSequence_Begin(struct Player *, s32, s32);

#define Player_BeginDirectionSequence sub_08158DF0

void Player_BeginDirectionSequence(struct Player *p, u32 axis)
{
    ActorSequence_Begin(p, 4, 1);
    p->coordinateAxis = axis;
    p->facing = (axis + 4) & 7;
    if (p->coordinateAxis > 4) {
        p->heading = (8 - p->coordinateAxis) >> 1;
        p->side = 1;
    } else {
        p->heading = p->coordinateAxis >> 1;
        p->side = 0;
    }
    p->directionFrame = 0;
}
