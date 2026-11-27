#include "npc/story_character/state_actor.h"
void Otenko_UpdateAnimation23(void *context, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x17;
    }
    if (p->done) {
        p->nextState = 1;
        Trigger_ConsumePending(p->receiver, 1);
    }
    p->timer += 1;
}
