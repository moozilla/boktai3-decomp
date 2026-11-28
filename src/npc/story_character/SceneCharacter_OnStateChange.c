#include "npc/story_character/state_actor.h"

extern u32 gUnk_086121C4[];

s32 SceneCharacter_OnStateChange(struct StateActor *p)
{
    s32 result;
    if (p->nextState >= 1 && p->nextState <= 0x60) {
        void (*callback)(void) = (void (*)(void))gUnk_086121C4[p->nextState];
        p->prevState = p->nextState;
        p->stateCallback = callback;
        p->stateFrame = 0;
        p->enter = 1;
        p->phase = 0;
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
