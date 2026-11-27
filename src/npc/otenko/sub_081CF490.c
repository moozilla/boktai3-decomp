#include "npc/otenko/otenko_actor.h"

void Otenko_UpdateBlinkPhase(void *unused, struct OtenkoActor *npc)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        npc->animationPhase = 1;
    }
    if ((u8)Callback_InvokeActor(npc, npc->animationCallback) != 1) {
        if (npc->animationPhase > 16)
            npc->done = 1;
        else {
            if (npc->animationPhase & 1) {
                u32 mask = 0x100;
                npc->displayFlags = mask | npc->displayFlags;
            }
            if (npc->stateFrame > npc->animationPhase) {
                npc->stateFrame = 0;
                npc->animationPhase = npc->animationPhase + 1;
            }
        }
    }
}
