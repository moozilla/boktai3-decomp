#include "npc/otenko/otenko_actor.h"

extern u32 gUnk_08612568[];

s32 Otenko_OnStateChange(struct OtenkoActor *p)
{
    s32 result;
    if (p->nextState >= 1 && p->nextState <= 0x22) {
        void (*callback)(void) = (void (*)(void))gUnk_08612568[p->nextState];
        p->prevState = p->nextState;
        p->stateCallback = callback;
        p->stateFrame = 0;
        p->animationEntered = 1;
        p->animationPhase = 0;
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
