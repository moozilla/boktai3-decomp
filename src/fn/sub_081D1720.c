#define SceneNpc_SetTargetToCurrentPosition sub_081D1720
#include "global.h"
#include "state_actor.h"
void sub_081D1720(void *ctx, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->target = p->position;
        p->nextState = 0xE;
        Trigger_ConsumePending(p->receiver, 1);
    }
}
