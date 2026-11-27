#include "npc/story_character/state_actor.h"
void Otenko_UpdateAnimation3AtCurrentPosition(void *context, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->target = p->position;
        p->nextState = 3;
        Trigger_ConsumePending(p->receiver, 1);
    }
    p->timer += 1;
}
