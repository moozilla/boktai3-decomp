#include "npc/story_character/state_actor.h"
void Otenko_UpdateAnimation17(void *context, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x11;
    }
    if (p->done) {
        p->state = 0;
        p->timer = 0;
        p->enterCommand = 1;
        Trigger_ConsumePending(p->receiver, 1);
    }
}
