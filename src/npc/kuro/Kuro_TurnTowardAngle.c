#include "global.h"
#include "npc/story_character/state_actor.h"
#include "scene/motion/scene_facing.h"
static inline void facing(struct StateActor *npc, short angle)
{
    npc->angle = angle;
    npc->facing8 = SceneFacing_EightWay(angle);
    npc->facing4 = SceneFacing_FourWay(angle);
}
void Kuro_PlayTaggedAnimation(void *, int, int, int, int);

void Kuro_TurnTowardAngle(void *ctx, struct StateActor *npc)
{
    s32 diff;
    if (npc->enter) {
        npc->enter = 0;
        npc->done = 0;
        Kuro_PlayTaggedAnimation(npc, 0, 1, 0, 4);
    }
    diff = npc->targetAngle - npc->angle;
    if (abs(diff) <= 8)
        npc->angle += diff;
    else {
        diff = (u8)diff;
        if (diff > 127)
            npc->angle -= 8;
        else
            npc->angle += 8;
    }
    facing(npc, npc->angle);
    Kuro_PlayTaggedAnimation(npc, 0, 1, 0, 4);
    if (npc->angle == npc->targetAngle)
        npc->done = 1;
}
