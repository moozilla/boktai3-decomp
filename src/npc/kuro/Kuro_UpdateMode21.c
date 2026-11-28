#include "npc/story_character/state_actor.h"

void Kuro_UpdateMode21(void *ctx, struct StateActor *p)
{
    struct StateActorTag *tag = &p->tag;
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x12;
        Trigger_ConsumePending(p->receiver, 1);
        tag->field16 = 1;
    }
}
