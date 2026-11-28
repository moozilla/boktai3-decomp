#include "npc/story_character/state_actor.h"

void Kuro_UpdateMode18(void *ctx, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0xF;
    }
    if (p->done) {
        p->nextState = 0x12;
        Trigger_ConsumePending(p->receiver, 1);
    }
}
