#include "npc/otenko/otenko_actor.h"
#include "scene/terrain/terrain_sampling.h"
// confidence=low: animation 13 rising phase that settles onto terrain height plus 160.
void Otenko_UpdateRisingToTerrain(void *ctx, struct OtenkoActor *npc, s32 tick)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 13, 2, 0, 4);
        npc->animationPhase = 0;
    }
    {
        u32 mask = 0x409;
        npc->displayFlags |= mask;
    }
    if ((u8)npc->animationCallback(npc) != 1) {
        switch (npc->animationPhase) {
            case 0:
                if (tick > 15)
                    npc->animationPhase = 1;
                break;
            case 1: {
                s32 h = Terrain_SampleHeight(&npc->body.position);
                if (npc->body.position.y >= h + 160) {
                    npc->body.position.y = h + 160;
                    npc->animationPhase++;
                } else {
                    npc->body.motion.y = 8;
                }
            } break;
            case 2:
                npc->done = 1;
                break;
        }
    }
}
