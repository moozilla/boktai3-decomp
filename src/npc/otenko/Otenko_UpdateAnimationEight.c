#include "npc/otenko/otenko_actor.h"

void Otenko_UpdateAnimationEight(void *context, struct OtenkoActor *npc)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 8, 1, 0, 4);
    }
    Callback_InvokeActor(npc, npc->animationCallback);
}
