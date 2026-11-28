#include "npc/otenko/otenko_actor.h"

void Otenko_PlayAnimation12(void *context, struct OtenkoActor *npc)
{
    struct OtenkoEffectState *effect = &npc->effect;
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 12, 0, 0, 4);
        effect->bobOffset = 0;
    }
    {
        u32 mask = 0x409;
        npc->displayFlags |= mask;
    }
    if ((u8)npc->animationCallback(npc) != 1)
        npc->done = 1;
}
