#include "global.h"
#include "npc/otenko/otenko_actor.h"
static inline void facing(struct OtenkoActor *npc, u32 angle)
{
    npc->body.direction = angle;
    {
        s32 t = (s32)(angle + 0x70) >> 5;
        u32 m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = (s32)(angle + 0x60) >> 6;
        u32 m = 3;
        m &= t;
        npc->facing4 = m;
    }
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
