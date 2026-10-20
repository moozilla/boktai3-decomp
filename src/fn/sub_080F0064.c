#include "global.h"
#include "scene_actor_chase.h"

void Ivy_UpdateActiveState(struct SceneActorRegistryResetView *);

#define Ivy_OnFlagDone sub_080F0064

void Ivy_OnFlagDone(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1)
        actor->field2B1 = 0;
    if (actor->field2B0) {
        Chase_SetCtl(actor, Ivy_UpdateActiveState, 0x22);
        Chase_SetState(actor, 0, 0, 2);
    }
}
