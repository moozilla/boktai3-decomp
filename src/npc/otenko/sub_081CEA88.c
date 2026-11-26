#include "npc/story_character/state_actor.h"
void Otenko_UpdateAnimation18(void *context, struct StateActor *p)
{
    if (p->enterCommand != 0) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x12;
    }
    if (p->done != 0) {
        u32 o = 1;
        p->state = o;
        p->timer = 0;
        p->enterCommand = o;
        Trigger_ConsumePending(p->receiver, 1);
    }
}
