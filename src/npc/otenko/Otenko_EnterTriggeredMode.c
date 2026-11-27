#include "npc/manager/scene_actor_lifecycle.h"
void Otenko_EnterTriggeredMode(void *context, struct SceneActor *p)
{
    if (p->commandEntered) {
        p->commandEntered = 0;
        p->finished = 0;
        p->action = 16;
        Trigger_ConsumePending(p->messageReceiver, 1);
    }
    p->savedState = p->state;
    if (p->finished) {
        p->state = 10;
        p->ticks = 0;
        p->commandEntered = 1;
    }
}
