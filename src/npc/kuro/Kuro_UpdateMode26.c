#include "npc/story_character/state_actor.h"

void Kuro_UpdateMode26(void *ctx, struct StateActor *p)
{
    struct StateActorTag *tag = &p->tag;
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x17;
        Trigger_ConsumePending(p->receiver, 1);
        tag->field16 = 1;
    }
}
