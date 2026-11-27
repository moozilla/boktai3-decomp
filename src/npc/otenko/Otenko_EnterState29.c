#include "npc/story_character/state_actor.h"
void Otenko_EnterState29(void *context, struct StateActor *p)
{
    if (p->enterCommand != 0) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x1d;
        Trigger_ConsumePending(p->receiver, 1);
    }
}
