#include "npc/story_character/state_actor.h"
void Otenko_UpdateAnimation20(void *context, struct StateActor *p)
{
    if (p->enterCommand != 0) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x14;
    }
    if (p->done != 0)
        Trigger_ConsumePending(p->receiver, 1);
    p->timer++;
}
