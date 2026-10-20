#include "global.h"
#include "scene_actor_chase.h"

void Ivy_BeginRelease(struct SceneActorRegistryResetView *);

#define Ivy_OnTimerDone sub_080EFFC0

void Ivy_OnTimerDone(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1)
        actor->field2B1 = 0;
    if (actor->field2B0) {
        Chase_SetCtl(actor, Ivy_BeginRelease, 0x29);
        Chase_SetState(actor, 0, 2, 2);
    }
}
