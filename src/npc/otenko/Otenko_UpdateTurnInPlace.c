#include "global.h"
#include "npc/otenko/otenko_actor.h"
#include "scene/motion/scene_facing.h"
static inline void facing(struct OtenkoActor *npc, u32 angle)
{
    npc->body.direction = angle;
    npc->facing8 = SceneFacing_EightWay(angle);
    npc->facing4 = SceneFacing_FourWay(angle);
}
void Otenko_UpdateTurnInPlace(void *ctx, struct OtenkoActor *npc)
{
    u8 facing8;
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 0, 0, 0, 4);
        npc->animationPhase = 0;
    }
    if ((u8)npc->animationCallback(npc) == 1)
        return;
    facing8 = npc->facing8;
    switch (npc->animationPhase) {
        case 2:
            facing(npc, (npc->body.direction + 4) & 0xff);
            if (npc->stateFrame & 0x20) {
                npc->stateFrame = 0;
                npc->animationPhase++;
            }
            break;
        case 0:
        case 4:
            facing(npc, (npc->body.direction - 4) & 0xff);
        case 1:
        case 3:
            if (npc->stateFrame & 0x10) {
                npc->stateFrame = 0;
                npc->animationPhase++;
            }
            break;
        case 5:
            npc->done = 1;
            break;
    }
    if (facing8 != npc->facing8)
        Actor_PlayAnimationEntry(npc, 0, 0, 0, 4);
}
