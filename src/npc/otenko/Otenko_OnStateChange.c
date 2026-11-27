#include "npc/otenko/otenko_actor.h"

extern u32 gUnk_08612568[];

s32 Otenko_OnStateChange(struct OtenkoActor *p)
{
    s32 r;
    if ((u8)(p->nextState - 1) <= 0x21) {
        void (*t)(void) = (void (*)(void))gUnk_08612568[p->nextState];
        u8 v = p->nextState;
        p->prevState = v;
        p->stateCallback = t;
        p->stateFrame = 0;
        p->animationEntered = 1;
        p->animationPhase = 0;
        r = 1;
    } else {
        r = 0;
    }
    return r;
}
