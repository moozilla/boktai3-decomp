#include "global.h"
#include "npc/story_character/state_actor.h"
void Kuro_SetTargetToCurrentPosition(void *ctx, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->target = p->position;
        p->nextState = 0xE;
        Trigger_ConsumePending(p->receiver, 1);
    }
}
