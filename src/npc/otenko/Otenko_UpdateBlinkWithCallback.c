#include "npc/otenko/otenko_actor.h"

void Otenko_UpdateBlinkWithCallback(void *ctx, struct OtenkoActor *npc)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        npc->timer = 16;
    }
    if ((u8)npc->animationCallback(npc) != 1) {
        u8 phase = npc->animationPhase;
        if (phase & 1) {
            u32 mask = 0x100;
            npc->displayFlags = mask | npc->displayFlags;
        }
        if (npc->timer) {
            if (--npc->timer == 0) {
                npc->animationPhase = (phase + 1) & 1;
                npc->timer = 16;
            }
        }
    }
}
