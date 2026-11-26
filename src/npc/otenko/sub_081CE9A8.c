#include "npc/story_character/state_actor.h"
void Otenko_UpdateAnimation14(void *context, struct StateActor *p)
{
    if (p->enterCommand != 0) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0xe;
    }
    if (p->done != 0) {
        u32 o = 1;
        p->state = o;
        p->timer = 0;
        p->enterCommand = o;
        Trigger_ConsumePending(p->receiver, 1);
    }
}
